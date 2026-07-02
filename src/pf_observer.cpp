/**
 * @file pf_observer.cpp
 * @brief ④ Observer / Event Dispatcher の実装。
 *
 * observer_dispatcher クラスが購読者テーブルを保持する単一インスタンス（静的確保）。
 * 配信は「開始時点の登録集合のスナップショット」に対して行い、配信中の購読変更
 * （再入）に対して安全にする。
 */
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_core.h"

namespace {

class observer_dispatcher {
public:
    pf_result_t init();
    pf_result_t subscribe(pf_state_id_t id, pf_observer_cb cb, void* ctx, pf_subscription_t* out_handle);
    pf_result_t unsubscribe(pf_subscription_t handle);
    pf_result_t dispatch(const pf_event_t& ev);

private:
    struct subscription {
        bool           used = false;
        pf_state_id_t  id   = 0;
        pf_observer_cb cb   = nullptr;
        void*          ctx  = nullptr;
    };

    void lock() const;
    void unlock() const;

    bool         init_ = false;
    subscription subs_[PF_OBSERVER_MAX]{};
};

void observer_dispatcher::lock() const   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
void observer_dispatcher::unlock() const { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

pf_result_t observer_dispatcher::init()
{
    lock();
    for (auto& s : subs_) s.used = false;
    init_ = true;
    unlock();
    return PF_OK;
}

pf_result_t observer_dispatcher::subscribe(pf_state_id_t id, pf_observer_cb cb, void* ctx, pf_subscription_t* out_handle)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!cb || !out_handle) return PF_ERR_INVALID_ARG;

    pf_result_t r = PF_ERR_NO_SPACE;
    lock();
    for (size_t i = 0; i < PF_OBSERVER_MAX; ++i) {
        if (!subs_[i].used) {
            subs_[i].used = true;
            subs_[i].id   = id;
            subs_[i].cb   = cb;
            subs_[i].ctx  = ctx;
            *out_handle = static_cast<pf_subscription_t>(i);
            r = PF_OK;
            break;
        }
    }
    unlock();
    return r;
}

pf_result_t observer_dispatcher::unsubscribe(pf_subscription_t handle)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (handle < 0 || static_cast<size_t>(handle) >= PF_OBSERVER_MAX) return PF_ERR_INVALID_ARG;
    if (!subs_[handle].used) return PF_ERR_NOT_FOUND;
    lock();
    subs_[handle].used = false;  /* 自分のコールバック内からでも安全（スナップショット配信のため） */
    unlock();
    return PF_OK;
}

pf_result_t observer_dispatcher::dispatch(const pf_event_t& ev)
{
    if (!init_) return PF_ERR_NOT_INIT;

    /* 配信対象を critical 内でスナップショット → 解放後に同期呼出（再入安全） */
    pf_observer_cb snap_cb[PF_OBSERVER_MAX];
    void*          snap_ctx[PF_OBSERVER_MAX];
    size_t         n = 0;

    lock();
    for (const auto& s : subs_) {
        if (s.used && (s.id == PF_STATE_ANY || s.id == ev.state_id)) {
            snap_cb[n]  = s.cb;
            snap_ctx[n] = s.ctx;
            ++n;
        }
    }
    unlock();

    for (size_t i = 0; i < n; ++i) {
        snap_cb[i](&ev, snap_ctx[i]);
    }
    return PF_OK;
}

observer_dispatcher g_observer;

} // namespace

pf_result_t pf_observer_init() { return g_observer.init(); }

pf_result_t pf_observer_subscribe(pf_state_id_t id, pf_observer_cb cb, void* ctx, pf_subscription_t* out_handle)
{
    return g_observer.subscribe(id, cb, ctx, out_handle);
}

pf_result_t pf_observer_unsubscribe(pf_subscription_t handle) { return g_observer.unsubscribe(handle); }

pf_result_t pf_observer_dispatch(const pf_event_t* ev)
{
    if (!ev) return PF_ERR_INVALID_ARG;
    return g_observer.dispatch(*ev);
}
