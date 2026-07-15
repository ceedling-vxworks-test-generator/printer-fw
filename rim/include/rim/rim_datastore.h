/*
 * rim_datastore.h - L2 RIM_DatastoreLayer 公開IF（C ABI）
 *
 * DataStore §6: CentralInputPortの3種post / 更新通知 / Snapshot提供。
 * 方針:
 *   - 層間IF（更新通知）は「関数ポインタテーブル＋ctx」（抽象クラス不使用・§2.4）。
 *   - Snapshotは has_xxx フラグ方式（optional不使用・§2.3）。
 *   - Registryは連番enum＋固定長配列で保持（§4-5）。
 */
#ifndef RIM_DATASTORE_H
#define RIM_DATASTORE_H

#include "rim/rim_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 更新通知IF（L3 RIM_CapabilityLayer が実装。関数ポインタテーブル＋ctx） ---- */
typedef struct rim_update_notifier {
    void  (*notify_updated)(void* ctx, rim_domain_set_t domains);
    void*  ctx;
} rim_update_notifier_t;

/* ---- Snapshot（読み取り専用・生成後不変・単一時点整合性） ---- */
typedef struct rim_fault_snapshot {
    uint32_t active_count;
    uint32_t active_codes[RIM_ID_COUNT];
} rim_fault_snapshot_t;

typedef struct rim_current_value_snapshot {
    rim_bool         present[RIM_ID_COUNT];
    rim_data_value_t values[RIM_ID_COUNT];
} rim_current_value_snapshot_t;

/* MachineSnapshot: optionalメンバは has_xxx フラグで表現 */
typedef struct rim_machine_snapshot {
    rim_bool                     has_fault;
    rim_fault_snapshot_t         fault;
    rim_bool                     has_current;
    rim_current_value_snapshot_t current;
} rim_machine_snapshot_t;

typedef struct rim_snapshot_request {
    rim_domain_set_t domains;
} rim_snapshot_request_t;

/* ---- ライフサイクル ---- */
/* Capability層の更新通知先を登録して初期化。notifierはNULL可（通知無効）。 */
rim_result_t rim_datastore_init(const rim_update_notifier_t* notifier);
void         rim_datastore_shutdown(void);

/* ---- CentralInputPort: 性質別3種post（DataStore §6.1） ---- */
/* post種別とid分類の一致・value型とidの一致を検証し、不一致は RIM_ERR_KIND_MISMATCH。 */
rim_result_t rim_datastore_post_fault(const rim_data_entry_item_t* item);
rim_result_t rim_datastore_post_operation(const rim_data_entry_item_t* item);
rim_result_t rim_datastore_post_current(const rim_data_entry_item_t* item);

/* ---- Dispatcher駆動（周期/イベント。スケルトンでは明示呼び出し） ---- */
/* 各レーンの取り込み→Registry反映→変化があれば notify_updated を発火。 */
void rim_datastore_dispatch(void);

/* ---- MachineSnapshotReader（Capability/Accessor が利用） ---- */
rim_result_t rim_datastore_capture(const rim_snapshot_request_t* req,
                                   rim_machine_snapshot_t* out_snapshot);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_DATASTORE_H */
