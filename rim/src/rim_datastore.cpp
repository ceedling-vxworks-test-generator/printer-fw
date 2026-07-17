/*
 * rim_datastore.cpp - L2 RIM_DatastoreLayer 実装（C-first ＋ 性質別キュー）
 *
 * DataStore §6/§8。性質(kind)ごとにステージング機構を使い分ける:
 *   - FAULT       : RingBuffer<FaultEvent> でFIFOステージ（喪失不可・順序保証）。
 *                   Registryは FixedMap<code, FaultEntry>（管理配列。add/remove/update）。
 *   - OPERATION   : RingBuffer<OpEvent> でFIFOステージ（喪失不可・順序保証）。
 *                   Registryは最新JobProgressの単一値。
 *   - CURRENT     : slot＋dirty の latest-wins（最新値優先。coalesceは機能・§6.4）。
 * 排他(H-7): 投入(post)・取り込み(dispatch)・取得(capture) を rim_port_lock で直列化。
 *   生産者はタスクのみ想定＝単一mutexで十分。dispatchは notify発火前に必ずunlockする
 *   （Capabilityの通知先が capture を再入呼びするため。ロック保持だと自己デッドロック）。
 */
#include "rim/rim_datastore.h"
#include "rim/rim_port.h"
#include "rim/rim_ring_buffer.h"
#include "rim/rim_fixed_map.h"

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

/* rim_fault_state_t → rim_collection_op_t 写像（後方互換）。 */
rim_collection_op_t map_fault_state(rim_fault_state_t s) {
    switch (s) {
        case RIM_FS_RAISED:         return RIM_OP_ADD;
        case RIM_FS_CLEARED:        return RIM_OP_REMOVE;
        case RIM_FS_ALL_CLEARED:    return RIM_OP_CLEAR_ALL;
        case RIM_FS_UPDATED_HEAL:   return RIM_OP_UPDATE;
        case RIM_FS_UPDATED_ACTIVE: return RIM_OP_UPDATE;
        case RIM_FS_NONE:           return RIM_OP_ADD;
        default:                    return RIM_OP_ADD;
    }
}

/* Faultコレクションの要素payload（heal/active等の付随情報）。 */
struct FaultEntry {
    uint8_t active;   /* 1=active, 0=healed（UPDATEで切替） */
};

/* FAULTレーンのFIFO要素（操作＋キー＋payload）。 */
struct FaultEvent {
    rim_collection_op_t op;
    uint32_t            key;
    FaultEntry          payload;
};

/* OPERATIONレーンのFIFO要素。 */
struct OpEvent {
    uint8_t job_progress;
};

struct CurrentSlot {
    rim_data_value_t value;
    rim_bool         present;
    rim_bool         dirty;   /* postで立てdispatchで落とす */
};

struct DatastoreState {
    rim_update_notifier_t notifier;
    rim_bool              has_notifier;

    /* CurrentValueRegistry / Buffer 兼用（latest-wins・スケルトン簡略） */
    CurrentSlot current[RIM_ID_COUNT];

    /* FAULTレーン: FIFO投入キュー ＋ 管理配列Registry */
    rim::RingBuffer<FaultEvent, RIM_FAULT_QUEUE_DEPTH> fault_ring;
    rim::FixedMap<uint32_t, FaultEntry, RIM_FAULT_CAP> fault_map;

    /* OPERATIONレーン: FIFO投入キュー ＋ 最新JobProgress */
    rim::RingBuffer<OpEvent, RIM_OP_QUEUE_DEPTH> op_ring;
    rim_bool op_present;
    uint8_t  op_job_progress;
};

DatastoreState g_ds;

void fire(rim_domain_set_t domains) {
    if (domains != RIM_DOMAIN_NONE && g_ds.has_notifier && g_ds.notifier.notify_updated)
        g_ds.notifier.notify_updated(g_ds.notifier.ctx, domains);
}

} /* namespace */

extern "C" {

rim_result_t rim_datastore_init(const rim_update_notifier_t* notifier) {
    for (int i = 0; i < RIM_ID_COUNT; ++i) {
        g_ds.current[i].present = RIM_FALSE;
        g_ds.current[i].dirty   = RIM_FALSE;
    }
    g_ds.fault_ring.clear();
    g_ds.fault_map.clear();
    g_ds.op_ring.clear();
    g_ds.op_present = RIM_FALSE;
    g_ds.op_job_progress = 0;
    if (notifier) { g_ds.notifier = *notifier; g_ds.has_notifier = RIM_TRUE; }
    else          { g_ds.has_notifier = RIM_FALSE; }
    return RIM_OK;
}

void rim_datastore_shutdown(void) { g_ds.has_notifier = RIM_FALSE; }

/* post_fault: 種別/値検証 → op・key・payload導出 → FIFOへ投入（満杯=RIM_ERR_POST）。 */
rim_result_t rim_datastore_post_fault(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_FAULT) return RIM_ERR_KIND_MISMATCH;

    /* op: has_op優先。無ければ fault_state から写像（未指定は RAISED=ADD 既定）。 */
    rim_collection_op_t op = item->context.has_op
        ? item->context.op
        : map_fault_state(item->context.has_fault_state ? item->context.fault_state : RIM_FS_RAISED);

    /*
     * 値型検証: CLEAR_ALL は値・キー不要。それ以外で has_key が無い場合のみ、
     * キー源として value が FAULT_CODE であることを要求する（従来pushパス互換）。
     * has_key 指定時（submit経由の REMOVE 等）は value=NONE を許容する。
     */
    if (op != RIM_OP_CLEAR_ALL && !item->context.has_key &&
        item->value.type != RIM_VT_FAULT_CODE) {
        return RIM_ERR_KIND_MISMATCH;
    }

    /* key: has_key優先。無ければ value.u.fault_code。 */
    uint32_t key = item->context.has_key ? item->context.key : item->value.u.fault_code;

    /* payload: heal/active（UPDATE時のみ意味を持つ。既定active）。 */
    FaultEntry payload;
    payload.active = (item->context.has_fault_state &&
                      item->context.fault_state == RIM_FS_UPDATED_HEAL) ? 0u : 1u;

    FaultEvent ev;
    ev.op = op; ev.key = key; ev.payload = payload;

    rim_result_t r = RIM_OK;
    rim_port_lock();
    if (!g_ds.fault_ring.push(ev)) r = RIM_ERR_POST;
    rim_port_unlock();
    return r;
}

rim_result_t rim_datastore_post_operation(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_OPERATION_REPORT) return RIM_ERR_KIND_MISMATCH;
    if (item->value.type != RIM_VT_JOB_PROGRESS) return RIM_ERR_KIND_MISMATCH;

    OpEvent ev;
    ev.job_progress = item->value.u.job_progress;

    rim_result_t r = RIM_OK;
    rim_port_lock();
    if (!g_ds.op_ring.push(ev)) r = RIM_ERR_POST;
    rim_port_unlock();
    return r;
}

rim_result_t rim_datastore_post_current(const rim_data_entry_item_t* item) {
    if (!item) return RIM_ERR_INVALID_ARG;
    rim_input_kind_t k;
    if (!expected_kind(item->id, &k) || k != RIM_KIND_CURRENT_VALUE) return RIM_ERR_KIND_MISMATCH;

    rim_port_lock();
    CurrentSlot* s = &g_ds.current[item->id];   /* 同一IDは最新で上書き（§6.4） */
    s->value = item->value;
    s->present = RIM_TRUE;
    s->dirty = RIM_TRUE;
    rim_port_unlock();
    return RIM_OK;
}

/* Dispatcher駆動: 各レーンのFIFO/slotを取り込み→Registry反映→変化ドメインを通知。 */
void rim_datastore_dispatch(void) {
    rim_domain_set_t changed = RIM_DOMAIN_NONE;

    rim_port_lock();

    /* FAULT: FIFO順に add/remove/update/clear_all を管理配列へ反映。 */
    FaultEvent fev;
    while (g_ds.fault_ring.pop(&fev)) {
        bool ch = false;
        switch (fev.op) {
            case RIM_OP_ADD:       ch = g_ds.fault_map.add(fev.key, fev.payload);    break;
            case RIM_OP_REMOVE:    ch = g_ds.fault_map.remove(fev.key);              break;
            case RIM_OP_UPDATE:    ch = g_ds.fault_map.update(fev.key, fev.payload); break;
            case RIM_OP_CLEAR_ALL: ch = (g_ds.fault_map.size() > 0); g_ds.fault_map.clear(); break;
            default: break;
        }
        if (ch) changed |= RIM_DOMAIN_FAULT;
    }

    /* OPERATION: FIFO順に最新JobProgressへ反映。 */
    OpEvent oev;
    while (g_ds.op_ring.pop(&oev)) {
        g_ds.op_job_progress = oev.job_progress;
        g_ds.op_present = RIM_TRUE;
        changed |= RIM_DOMAIN_OPERATION;
    }

    /* CURRENT: dirty を取り込み（latest-wins）。 */
    for (int i = 0; i < RIM_ID_COUNT; ++i) {
        if (g_ds.current[i].dirty) {
            g_ds.current[i].dirty = RIM_FALSE;  /* 取り込みでクリア（§6.4） */
            changed |= RIM_DOMAIN_CURRENT;
        }
    }

    rim_port_unlock();   /* ★ 通知発火の前に必ず解放（capture再入によるデッドロック回避） */

    fire(changed);
}

rim_result_t rim_datastore_capture(const rim_snapshot_request_t* req,
                                   rim_machine_snapshot_t* out) {
    if (!req || !out) return RIM_ERR_INVALID_ARG;
    out->has_fault = RIM_FALSE;
    out->has_current = RIM_FALSE;

    rim_port_lock();

    if (req->domains & RIM_DOMAIN_FAULT) {
        out->has_fault = RIM_TRUE;
        uint32_t n = (uint32_t)g_ds.fault_map.size();
        out->fault.active_count = n;
        for (uint32_t i = 0; i < n; ++i)
            out->fault.active_codes[i] = g_ds.fault_map.key_at(i);
    }
    if (req->domains & RIM_DOMAIN_CURRENT) {
        out->has_current = RIM_TRUE;
        for (int i = 0; i < RIM_ID_COUNT; ++i) {
            out->current.present[i] = g_ds.current[i].present;
            out->current.values[i]  = g_ds.current[i].value;
        }
    }

    rim_port_unlock();
    return RIM_OK;
}

} /* extern "C" */
