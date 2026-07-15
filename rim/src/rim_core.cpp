/*
 * rim_core.cpp - RIM 一括初期化・結線 と 隣接 Accessor Layer 実装
 *
 * 結線順: Publisher → Capability → DataStore(通知先=Capabilityのnotifier) → Adapter。
 * Accessor（RIM4レイヤ外の隣接・参照専用/Pull）は L2/L3 から都度取得して返す。
 */
#include "rim/rim.h"

extern "C" {

rim_result_t rim_init(void) {
    rim_result_t r;
    if ((r = rim_publisher_init())  != RIM_OK) return r;
    if ((r = rim_capability_init()) != RIM_OK) return r;
    /* DataStoreの更新通知先にCapabilityのnotifierを結線 */
    if ((r = rim_datastore_init(rim_capability_notifier())) != RIM_OK) return r;
    if ((r = rim_adapter_init())    != RIM_OK) return r;
    return RIM_OK;
}

void rim_shutdown(void) {
    rim_adapter_shutdown();
    rim_datastore_shutdown();
    rim_capability_shutdown();
    rim_publisher_shutdown();
}

/* --- 隣接 Accessor Layer（参照専用・Pull） --- */
rim_result_t rim_accessor_read_snapshot(rim_domain_set_t domains,
                                        rim_machine_snapshot_t* out) {
    rim_snapshot_request_t req;
    req.domains = domains;
    return rim_datastore_capture(&req, out);
}

rim_bool rim_accessor_read_capability(rim_capability_set_t* out) {
    return rim_capability_current(out);
}

} /* extern "C" */
