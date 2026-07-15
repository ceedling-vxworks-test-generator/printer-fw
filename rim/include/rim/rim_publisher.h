/*
 * rim_publisher.h - L4 RIM_PublisherLayer 公開IF（C ABI）
 *
 * Publisher §: 購読者へ Capability を Push 配信する（Pull参照は隣接 Accessor Layer が担当）。
 *   - 購読者は「コールバック関数ポインタ＋ctx」で登録（関数ポインタテーブル方針）。
 *   - 購読者リストは可変長Listの仕組み（rim_list.h＝C++ FixedVector）で保持し得る。
 */
#ifndef RIM_PUBLISHER_H
#define RIM_PUBLISHER_H

#include "rim/rim_types.h"
#include "rim/rim_capability.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rim_subscriber_fn)(void* ctx, const rim_capability_set_t* cap);

rim_result_t rim_publisher_init(void);
void         rim_publisher_shutdown(void);

/* 購読登録（満杯なら RIM_ERR_FULL）。 */
rim_result_t rim_publisher_subscribe(rim_subscriber_fn fn, void* ctx);

/* 全購読者へ Push 配信（L3から呼ばれる）。 */
void rim_publisher_publish(const rim_capability_set_t* cap);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_PUBLISHER_H */
