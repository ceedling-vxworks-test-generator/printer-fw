#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Capability の公開表現。
 *
 * Core は Capability の中身を「不透明なバイト列」としてしか扱わない。
 * 何番がどの構造体かを知っているのは、その機種を作った Product と、
 * その Product 向けにビルドされた利用者だけである。
 * (機種ごとの ID 一覧は Product が提供するヘッダを参照すること)
 */

/* Capability 識別子。値の割り当ては Product の責務。 */
typedef uint16_t RICapabilityId;

/*
 * Capability 通知コールバック。
 *
 * data は size バイトの Capability 本体を指す。**コールバックから戻るまでの間しか
 * 有効でない**ため、保持したい場合は呼出側でコピーすること。
 */
typedef void (*RICapabilityCallback)(
    uint64_t subscriptionId,
    RICapabilityId capabilityId,
    const void* data,
    size_t size,
    void* userData);

#ifdef __cplusplus
}
#endif
