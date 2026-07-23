/*
 * rim_adapter.cpp - L1 RIM_AdapterLayer 実装
 *
 * 要件4(b)の実体: 受理点の「型の自由さ」をC++テンプレートで吸収する箇所。
 *   - push_i32 / push_f / push_u32 は、native型の異なる入口。
 *   - 内部テンプレート intake<T>() が、型の違いを1本の正規化パスへ集約する。
 *   - Rule が native値 → 正規化済み rim_data_value_t（C表現）へ変換。
 *   - InputClassifier が id から性質を判別し、DataStoreの3種postを選択する。
 *
 * 「型の自由さ」以外（性質判別・分岐・後続呼び出し）は C的な素直な処理で書く。
 */
#include "rim/rim_adapter.h"
#include "rim/rim_datastore.h"

namespace {

/* --- InputClassifier: id → 性質（GAP-1）。設計不備時は判別不能。 --- */
bool classify(rim_data_id_t id, rim_input_kind_t* out) {
    switch (id) {
        case RIM_ID_FAULT_CODE:   *out = RIM_KIND_FAULT;            return true;
        case RIM_ID_JOB_PROGRESS: *out = RIM_KIND_OPERATION_REPORT; return true;
        case RIM_ID_TEMPERATURE:
        case RIM_ID_HUMIDITY:
        case RIM_ID_PRESSURE:     *out = RIM_KIND_CURRENT_VALUE;     return true;
        default: return false;
    }
}

/*
 * Rule: native型Tの生値を、id定義に沿った正規化 rim_data_value_t へ変換する。
 * ここがテンプレートで「型の自由さ」を吸収する中核。T は i32/double/u32 いずれでも
 * static_cast で受けられ、id ごとの正規化規則へ合流する。
 */
template <typename T>
bool convert(rim_data_id_t id, T raw, const rim_data_context_t* ctx, rim_data_value_t* out) {
    const int32_t scale = (ctx && ctx->has_scale) ? ctx->scale_x1000 : 1000;
    switch (id) {
        case RIM_ID_TEMPERATURE:
            out->type = RIM_VT_CELSIUS_X100;
            out->u.celsius_x100 = static_cast<int32_t>((static_cast<double>(raw) * scale / 1000.0) * 100.0);
            return true;
        case RIM_ID_HUMIDITY:
        case RIM_ID_PRESSURE: {
            long p = static_cast<long>(raw);
            if (p < 0) p = 0; if (p > 100) p = 100;
            out->type = RIM_VT_PERCENT;
            out->u.percent = static_cast<uint8_t>(p);
            return true;
        }
        case RIM_ID_JOB_PROGRESS: {
            long p = static_cast<long>(raw);
            if (p < 0) p = 0; if (p > 100) p = 100;
            out->type = RIM_VT_JOB_PROGRESS;
            out->u.job_progress = static_cast<uint8_t>(p);
            return true;
        }
        case RIM_ID_FAULT_CODE:
            out->type = RIM_VT_FAULT_CODE;
            out->u.fault_code = static_cast<uint32_t>(raw);
            return true;
        default:
            out->type = RIM_VT_NONE;
            return false;
    }
}

/* 正規化済みitemを性質に応じた3種postへ送る（RawDataInput詳細 §10）。 */
rim_result_t dispatch_post(rim_input_kind_t kind, const rim_data_entry_item_t* item) {
    switch (kind) {
        case RIM_KIND_FAULT:            return rim_datastore_post_fault(item);
        case RIM_KIND_OPERATION_REPORT: return rim_datastore_post_operation(item);
        case RIM_KIND_CURRENT_VALUE:    return rim_datastore_post_current(item);
        default:                        return RIM_ERR_CLASSIFY;
    }
}

/* 型自由な受理点の共通実装（intake）。native型Tを受け正規化して後続へ。 */
template <typename T>
rim_result_t intake(rim_data_id_t id, T raw, const rim_data_context_t* ctx) {
    if ((int)id < 0 || (int)id >= (int)RIM_ID_COUNT) return RIM_ERR_INVALID_ARG;

    rim_data_entry_item_t item;
    item.id = id;
    if (ctx) item.context = *ctx;
    else {
        item.context.has_fault_state = RIM_FALSE;
        item.context.fault_state     = RIM_FS_NONE;
        item.context.has_scale       = RIM_FALSE;
        item.context.scale_x1000     = 1000;
        item.context.has_op          = RIM_FALSE;
        item.context.op              = RIM_OP_NONE;
        item.context.has_key         = RIM_FALSE;
        item.context.key             = 0u;
    }

    if (!convert(id, raw, ctx, &item.value)) return RIM_ERR_CONVERT;

    rim_input_kind_t kind;
    if (!classify(id, &kind)) return RIM_ERR_CLASSIFY;

    return dispatch_post(kind, &item);
}

} /* namespace */

extern "C" {

rim_result_t rim_adapter_init(void)      { return RIM_OK; } /* 状態を持たない層 */
void         rim_adapter_shutdown(void)  { }

rim_result_t rim_adapter_push_i32(rim_data_id_t id, int32_t raw, const rim_data_context_t* ctx) {
    return intake<int32_t>(id, raw, ctx);
}
rim_result_t rim_adapter_push_f(rim_data_id_t id, double raw, const rim_data_context_t* ctx) {
    return intake<double>(id, raw, ctx);
}
rim_result_t rim_adapter_push_u32(rim_data_id_t id, uint32_t raw, const rim_data_context_t* ctx) {
    return intake<uint32_t>(id, raw, ctx);
}

/*
 * コレクション操作の受理点。型自由パス(intake)とは別に、op＋key＋任意valueを
 * そのままキューレーンへ流す（正規化変換は行わない＝valueは既に正規化済み前提）。
 */
rim_result_t rim_adapter_submit(rim_data_id_t id, rim_collection_op_t op,
                                uint32_t key, const rim_data_value_t* value,
                                const rim_data_context_t* ctx) {
    if ((int)id < 0 || (int)id >= (int)RIM_ID_COUNT) return RIM_ERR_INVALID_ARG;

    rim_input_kind_t kind;
    if (!classify(id, &kind)) return RIM_ERR_CLASSIFY;
    /* コレクション操作はキューレーン（FAULT/OPERATION）限定。CURRENTは非対応。 */
    if (kind != RIM_KIND_FAULT && kind != RIM_KIND_OPERATION_REPORT)
        return RIM_ERR_KIND_MISMATCH;

    rim_data_entry_item_t item;
    item.id = id;

    if (ctx) item.context = *ctx;
    else {
        item.context.has_fault_state = RIM_FALSE;
        item.context.fault_state     = RIM_FS_NONE;
        item.context.has_scale       = RIM_FALSE;
        item.context.scale_x1000     = 1000;
    }
    /* op/key は本引数で明示指定。 */
    item.context.has_op  = RIM_TRUE;
    item.context.op      = op;
    item.context.has_key = RIM_TRUE;
    item.context.key     = key;

    /* value: REMOVE/CLEAR_ALL は不要 → NONE。ADD/UPDATE は与えられた値。 */
    if (op == RIM_OP_REMOVE || op == RIM_OP_CLEAR_ALL || value == 0) {
        item.value.type = RIM_VT_NONE;
    } else {
        item.value = *value;
    }

    return dispatch_post(kind, &item);
}

} /* extern "C" */
