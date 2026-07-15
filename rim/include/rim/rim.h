/*
 * rim.h - ReactiveInfoManager アンブレラヘッダ（C ABI）
 *
 * L1 RIM_AdapterLayer → L2 RIM_DatastoreLayer → L3 RIM_CapabilityLayer → L4 RIM_PublisherLayer
 * ＋ 隣接 Accessor（参照専用/Pull）。すべて extern "C"。C言語からそのまま使用可。
 *
 * 全体像・命名・実装方針: ex/共通/ReactiveInfoManager概要.md
 */
#ifndef RIM_H
#define RIM_H

#include "rim/rim_types.h"
#include "rim/rim_list.h"
#include "rim/rim_adapter.h"
#include "rim/rim_datastore.h"
#include "rim/rim_capability.h"
#include "rim/rim_publisher.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * RIM一括初期化: Publisher→Capability→DataStore(通知先=Capability)→Adapter の順で結線する。
 * 個別 init を順に呼ぶのと等価だが、結線順の誤りを防ぐため本APIの利用を推奨する。
 */
rim_result_t rim_init(void);
void         rim_shutdown(void);

/* --- 隣接 Accessor Layer（参照専用・Pull）: RIMの4レイヤ外の隣接IF --- */
/* 現在の機械Snapshotを都度取得（保持しない）。 */
rim_result_t rim_accessor_read_snapshot(rim_domain_set_t domains,
                                        rim_machine_snapshot_t* out);
/* 現在のCapabilityを都度取得。 */
rim_bool     rim_accessor_read_capability(rim_capability_set_t* out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_H */
