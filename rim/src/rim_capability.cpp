/*
 * rim_capability.cpp - L3 RIM_CapabilityLayer 実装（C-first）
 *
 * Capability §: L2の更新通知を受け、MachineSnapshotを取得して意味づけ(Capability)を生成し、
 * L4 RIM_PublisherLayer へ配信する。
 *   - 更新通知IFは関数ポインタテーブル（notify_updatedを本層が実装）。
 *   - Evaluator群は関数ポインタ配列（§2.9）。純関数として状態を持たない。
 */
#include "rim/rim_capability.h"
#include "rim/rim_publisher.h"

namespace {

struct CapabilityState {
    rim_capability_set_t current;
    rim_bool             has_current;
};
CapabilityState g_cap;

/* Evaluator: Snapshot → 個別Capabilityフラグ。関数ポインタ配列で束ねる。 */
typedef void (*evaluator_fn)(const rim_machine_snapshot_t*, rim_capability_set_t*);

void eval_printable(const rim_machine_snapshot_t* s, rim_capability_set_t* out) {
    rim_bool no_fault = RIM_TRUE;
    if (s->has_fault && s->fault.active_count > 0) no_fault = RIM_FALSE;
    out->has_printable = RIM_TRUE;
    out->printable = no_fault;
}

void eval_temp_alert(const rim_machine_snapshot_t* s, rim_capability_set_t* out) {
    rim_bool alert = RIM_FALSE;
    if (s->has_current && s->current.present[RIM_ID_TEMPERATURE]) {
        const rim_data_value_t* v = &s->current.values[RIM_ID_TEMPERATURE];
        if (v->type == RIM_VT_CELSIUS_X100 && v->u.celsius_x100 > 6000) /* >60.00℃ */
            alert = RIM_TRUE;
    }
    out->has_temp_alert = RIM_TRUE;
    out->temp_alert = alert;
}

evaluator_fn g_evaluators[] = { eval_printable, eval_temp_alert };
const int g_evaluator_count = (int)(sizeof(g_evaluators) / sizeof(g_evaluators[0]));

/* L2の更新通知を受ける本体（notify_updated実装） */
void on_updated(void* /*ctx*/, rim_domain_set_t /*domains*/) {
    rim_snapshot_request_t req;
    req.domains = RIM_DOMAIN_FAULT | RIM_DOMAIN_CURRENT;

    rim_machine_snapshot_t snap;
    if (rim_datastore_capture(&req, &snap) != RIM_OK) return;

    rim_capability_set_t cap;
    cap.has_printable = RIM_FALSE; cap.printable = RIM_FALSE;
    cap.has_temp_alert = RIM_FALSE; cap.temp_alert = RIM_FALSE;
    for (int i = 0; i < g_evaluator_count; ++i) g_evaluators[i](&snap, &cap);

    g_cap.current = cap;
    g_cap.has_current = RIM_TRUE;

    rim_publisher_publish(&cap); /* L4へPush */
}

const rim_update_notifier_t g_notifier = { on_updated, 0 };

} /* namespace */

extern "C" {

rim_result_t rim_capability_init(void) {
    g_cap.has_current = RIM_FALSE;
    return RIM_OK;
}
void rim_capability_shutdown(void) { g_cap.has_current = RIM_FALSE; }

const rim_update_notifier_t* rim_capability_notifier(void) { return &g_notifier; }

rim_bool rim_capability_current(rim_capability_set_t* out) {
    if (!out || !g_cap.has_current) return RIM_FALSE;
    *out = g_cap.current;
    return RIM_TRUE;
}

} /* extern "C" */
