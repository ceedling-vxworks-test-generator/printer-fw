/*
 * rim_publisher.cpp - L4 RIM_PublisherLayer 実装（C-first）
 *
 * Publisher §: 購読者へ Capability を Push 配信。
 *   - 購読者は関数ポインタ＋ctxの固定長配列で保持（連番/固定長方針）。
 *     可変長が必要な箇所は rim_list.h（C++ FixedVector）へ委譲できる。
 */
#include "rim/rim_publisher.h"

namespace {

#ifndef RIM_MAX_SUBSCRIBERS
#define RIM_MAX_SUBSCRIBERS 8
#endif

struct Subscriber {
    rim_subscriber_fn fn;
    void*             ctx;
};

struct PublisherState {
    Subscriber subs[RIM_MAX_SUBSCRIBERS];
    int        count;
};
PublisherState g_pub;

} /* namespace */

extern "C" {

rim_result_t rim_publisher_init(void) { g_pub.count = 0; return RIM_OK; }
void         rim_publisher_shutdown(void) { g_pub.count = 0; }

rim_result_t rim_publisher_subscribe(rim_subscriber_fn fn, void* ctx) {
    if (!fn) return RIM_ERR_INVALID_ARG;
    if (g_pub.count >= RIM_MAX_SUBSCRIBERS) return RIM_ERR_FULL;
    g_pub.subs[g_pub.count].fn = fn;
    g_pub.subs[g_pub.count].ctx = ctx;
    ++g_pub.count;
    return RIM_OK;
}

void rim_publisher_publish(const rim_capability_set_t* cap) {
    if (!cap) return;
    for (int i = 0; i < g_pub.count; ++i)
        g_pub.subs[i].fn(g_pub.subs[i].ctx, cap);
}

} /* extern "C" */
