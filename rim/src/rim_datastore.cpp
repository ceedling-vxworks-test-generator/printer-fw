/*
 * rim_datastore.cpp - L2 RIM_DatastoreLayer 実装（C-first）
 *
 * DataStore §6/§8。C的な素直な実装:
 *   - Registryは連番enum＋固定長配列（§4-5）。optional不使用・has_xxxフラグ（§2.3）。
 *   - CurrentValueBufferはslot＋dirty（§6.4）。post種別とid分類の一致を検証（§6.1）。
 *   - 更新通知は関数ポインタテーブル rim_update_notifier_t（§2.4）。
 *   - スケルトンにつき排他は簡略化（実機はport mutexで短時間ロック＝H-7緩和）。
 */
#include "rim/rim_datastore.h"

namespace {

/* id → 期待する性質（post検証用。Adapterのclassifyと同一定義） */
bool expected_kind(rim_data_id_t id, rim_input_kind_t* out) {
    switch (id) {
        case RIM_ID_FAULT_CODE:   *out = RIM_KIND_FAULT;            return true;
        case RIM_ID_JOB_PROGRESS: *out = RIM_KIND_OPERATION_REPORT; return true;
        case RIM_ID_TEMPERATURE:
        case RIM_ID_HUMIDITY:
        case RIM_ID_PRESSURE:     *out = RIM_KIND_CURRENT_VALUE;     return true;
        default: return false;
    }
}

struct CurrentSlot {
    rim_data_value_t value;
    rim_bool         present;
    rim_bool         dirty;   /* postで立てdispatchで落とす */
};

struct FaultPending {
    rim_bool          pending;
    rim_data_id_t     id;
    rim_fault_state_t state;
    uint32_t          code;
};

struct DatastoreState {
    rim_update_notifier_t notifier;
    rim_bool              has_notifier;

    /* CurrentValueRegistry / Buffer 兼用（スケルトン簡略） */
    CurrentSlot current[RIM_ID_COUNT];

    /* FaultRegistry */
    uint32_t fault_codes[RIM_ID_COUNT];
    uint32_t fault_count;
    FaultPending fault_in;

    /* OperationRegistry（最新JobProgress） */
    rim_bool     op_pending;
    uint8_t      op_job_progress;
    rim_bool     op_present;
};

DatastoreState g_ds;

void fire(rim_domain_set_t domains) {
    if (domains != RIM_DOMAIN_NONE && g_ds.has_notifier && g_ds.notifier.notify_updated)
        g_ds.notifier.notify_updated(g_ds.notifier.ctx, domains);
}

bool fault_raise(uint32_t code) {
    for (uint32_t i = 0; i < g_ds.fault_count; ++i)
        if (g_ds.fault_codes[i] == code) return false; /* 既登録=変化なし */
    if (g_ds.fault_count >= RIM_ID_COUNT) return false;
    g_ds.fault_codes[g_ds.fault_count++] = code;
    return true;
}
bool fault_clear(uint32_t code) {
    for (uint32_t i = 0; i < g_ds.fault_count; ++i) {
        if (g_ds.fault_codes[i] == code) {
            g_ds.fault_codes[i] = g_ds.fault_codes[--g_ds.fault_count];
            return true;
        }
    }
    return false;
}

} /* namespace */

extern "C" {

rim_result_t rim_datastore_init(const rim_update_notifier_t* notifier) {
    for (int i = 0; i < RIM_ID_COUNT; ++i) {
        g_ds.current[i].present = RIM_FALSE;
        g_ds.current[i].dirty   = RIM_FALSE;
        g_ds.fault_codes[i]     = 0;
    }
    g_ds.fault_count = 0;
    g_ds.fault_in.pending = RIM_FALSE;
    g_ds.op_pending = RIM_FALSE;
    g_ds.op_present = RIM_FALSE;
    g_ds.op_job_progress = 0;
    if (notifier) { g_ds.notifier = *notifier; g_ds.has_notifier = RIM_TRUE; }
    else          { g_ds.has_notifier = RIM_FALSE; }
    return RIM_OK;
}

void rim_datastore_shutdown(void) { g_ds.has_notifier = RIM_FALSE; }

/* post: 種別とidの一致検証（§6.1）→ Queue/Buffer相当へ格納 */
rim_result_t rim_datastore_post_fault(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_FAULT) return RIM_ERR_KIND_MISMATCH;
    if (item->value.type != RIM_VT_FAULT_CODE) return RIM_ERR_KIND_MISMATCH;
    g_ds.fault_in.pending = RIM_TRUE;
    g_ds.fault_in.id      = item->id;
    g_ds.fault_in.state   = item->context.has_fault_state ? item->context.fault_state : RIM_FS_RAISED;
    g_ds.fault_in.code    = item->value.u.fault_code;
    return RIM_OK;
}

rim_result_t rim_datastore_post_operation(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_OPERATION_REPORT) return RIM_ERR_KIND_MISMATCH;
    if (item->value.type != RIM_VT_JOB_PROGRESS) return RIM_ERR_KIND_MISMATCH;
    g_ds.op_pending = RIM_TRUE;
    g_ds.op_job_progress = item->value.u.job_progress;
    return RIM_OK;
}

rim_result_t rim_datastore_post_current(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_CURRENT_VALUE) return RIM_ERR_KIND_MISMATCH;
    CurrentSlot* s = &g_ds.current[item->id];   /* 同一IDは最新で上書き（§6.4） */
    s->value = item->value;
    s->present = RIM_TRUE;
    s->dirty = RIM_TRUE;
    return RIM_OK;
}

/* Dispatcher駆動: pending取り込み→Registry反映→変化ドメインを通知 */
void rim_datastore_dispatch(void) {
    rim_domain_set_t changed = RIM_DOMAIN_NONE;

    if (g_ds.fault_in.pending) {
        g_ds.fault_in.pending = RIM_FALSE;
        bool ch = false;
        switch (g_ds.fault_in.state) {
            case RIM_FS_RAISED:        ch = fault_raise(g_ds.fault_in.code);  break;
            case RIM_FS_CLEARED:       ch = fault_clear(g_ds.fault_in.code);  break;
            case RIM_FS_ALL_CLEARED:   ch = (g_ds.fault_count > 0); g_ds.fault_count = 0; break;
            case RIM_FS_UPDATED_HEAL:
            case RIM_FS_UPDATED_ACTIVE: ch = true; break; /* スケルトン: 状態更新は変化扱い */
            default: break;
        }
        if (ch) changed |= RIM_DOMAIN_FAULT;
    }

    if (g_ds.op_pending) {
        g_ds.op_pending = RIM_FALSE;
        rim_bool was = g_ds.op_present;
        g_ds.op_present = RIM_TRUE;
        changed |= RIM_DOMAIN_OPERATION; /* スケルトン: 反映を変化扱い */
        (void)was;
    }

    for (int i = 0; i < RIM_ID_COUNT; ++i) {
        if (g_ds.current[i].dirty) {
            g_ds.current[i].dirty = RIM_FALSE;  /* 取り込みでクリア（§6.4） */
            changed |= RIM_DOMAIN_CURRENT;
        }
    }

    fire(changed);
}

rim_result_t rim_datastore_capture(const rim_snapshot_request_t* req,
                                   rim_machine_snapshot_t* out) {
    if (!req || !out) return RIM_ERR_INVALID_ARG;
    out->has_fault = RIM_FALSE;
    out->has_current = RIM_FALSE;

    if (req->domains & RIM_DOMAIN_FAULT) {
        out->has_fault = RIM_TRUE;
        out->fault.active_count = g_ds.fault_count;
        for (uint32_t i = 0; i < g_ds.fault_count; ++i)
            out->fault.active_codes[i] = g_ds.fault_codes[i];
    }
    if (req->domains & RIM_DOMAIN_CURRENT) {
        out->has_current = RIM_TRUE;
        for (int i = 0; i < RIM_ID_COUNT; ++i) {
            out->current.present[i] = g_ds.current[i].present;
            out->current.values[i]  = g_ds.current[i].value;
        }
    }
    return RIM_OK;
}

} /* extern "C" */
