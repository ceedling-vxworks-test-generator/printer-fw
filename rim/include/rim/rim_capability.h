/*
 * rim_capability.h - L3 RIM_CapabilityLayer 公開IF（C ABI）
 *
 * Capability §: L2の更新通知(notifyUpdated)を受け、Snapshotを取得して意味づけ(Capability)を
 * 生成し、L4 RIM_PublisherLayer へ配信する。
 *   - 更新通知IFは rim_update_notifier_t（関数ポインタテーブル）として L2 に登録する。
 *   - 判定式(Evaluator)群は関数ポインタ配列で表現（抽象クラス不使用・§2.9）。
 */
#ifndef RIM_CAPABILITY_H
#define RIM_CAPABILITY_H

#include "rim/rim_types.h"
#include "rim/rim_datastore.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 生成されるCapability（意味づけ結果）。スケルトンでは代表2種を持つ。 */
typedef struct rim_capability_set {
    rim_bool has_printable;  rim_bool printable;   /* 印刷可否 */
    rim_bool has_temp_alert; rim_bool temp_alert;  /* 温度警報 */
} rim_capability_set_t;

rim_result_t rim_capability_init(void);
void         rim_capability_shutdown(void);

/* L2 へ渡す更新通知IF（notify_updated 実装）を取得する。 */
const rim_update_notifier_t* rim_capability_notifier(void);

/* 直近に生成したCapabilityをコピー取得（Accessor経由のPull等で利用）。 */
rim_bool rim_capability_current(rim_capability_set_t* out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_CAPABILITY_H */
