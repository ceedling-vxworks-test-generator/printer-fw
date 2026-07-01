/**
 * @file pf_observer.c
 * @brief ④ Observer / Event Dispatcher の実装スケルトン。
 *
 * 静的な購読者テーブル。配信は「開始時点の登録集合のスナップショット」に対して行い、
 * 配信中の購読変更（再入）に対して安全にする。
 */
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_core.h"   /* pf_core_port() */

typedef struct {
    bool           used;
    pf_state_id_t  id;       /* PF_STATE_ANY で全状態 */
    pf_observer_cb cb;
    void*          ctx;
} sub_t;

static bool  g_init;
static sub_t g_subs[PF_OBSERVER_MAX];

static void lock(void)   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
static void unlock(void) { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

pf_result_t pf_observer_init(void)
{
    lock();
    for (size_t i = 0; i < PF_OBSERVER_MAX; ++i) g_subs[i].used = false;
    g_init = true;
    unlock();
    return PF_OK;
}

pf_result_t pf_observer_subscribe(pf_state_id_t id, pf_observer_cb cb,
                                  void* ctx, pf_subscription_t* out_handle)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!cb || !out_handle) return PF_ERR_INVALID_ARG;

    pf_result_t r = PF_ERR_NO_SPACE;
    lock();
    for (size_t i = 0; i < PF_OBSERVER_MAX; ++i) {
        if (!g_subs[i].used) {
            g_subs[i].used = true;
            g_subs[i].id   = id;
            g_subs[i].cb   = cb;
            g_subs[i].ctx  = ctx;
            *out_handle = (pf_subscription_t)i;
            r = PF_OK;
            break;
        }
    }
    unlock();
    return r;
}

pf_result_t pf_observer_unsubscribe(pf_subscription_t handle)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (handle < 0 || (size_t)handle >= PF_OBSERVER_MAX) return PF_ERR_INVALID_ARG;
    if (!g_subs[handle].used) return PF_ERR_NOT_FOUND;
    lock();
    g_subs[handle].used = false;  /* 自分のコールバック内からでも安全（スナップショット配信のため） */
    unlock();
    return PF_OK;
}

pf_result_t pf_observer_dispatch(const pf_event_t* ev)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!ev) return PF_ERR_INVALID_ARG;

    /* 配信対象を critical 内でスナップショット → 解放後に同期呼出（再入安全） */
    pf_observer_cb snap_cb[PF_OBSERVER_MAX];
    void*          snap_ctx[PF_OBSERVER_MAX];
    size_t         n = 0;

    lock();
    for (size_t i = 0; i < PF_OBSERVER_MAX; ++i) {
        if (g_subs[i].used &&
            (g_subs[i].id == PF_STATE_ANY || g_subs[i].id == ev->state_id)) {
            snap_cb[n]  = g_subs[i].cb;
            snap_ctx[n] = g_subs[i].ctx;
            ++n;
        }
    }
    unlock();

    for (size_t i = 0; i < n; ++i) {
        snap_cb[i](ev, snap_ctx[i]);
    }
    return PF_OK;
}
