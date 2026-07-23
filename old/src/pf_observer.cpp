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

/**
 * @brief Observer（購読者管理・配信）の実体。
 *
 * 購読者テーブルは固定長配列（PF_OBSERVER_MAX件）で、Subscribe/Unsubscribe/dispatch
 * の最小インターフェースを提供する。
 */
class observer_dispatcher {
public:
    /** @brief 購読者テーブルをクリアして使用可能にする。@return 常にPF_OK */
    pf_result_t init();
    /** @brief 状態IDを購読する。空きスロットが無ければ失敗する。@return subscribe結果 */
    pf_result_t subscribe(pf_state_id_t id, pf_observer_cb cb, void* ctx, pf_subscription_t* out_handle);
    /** @brief 購読を解除する（コールバック内からの自己解除も安全）。@return unsubscribe結果 */
    pf_result_t unsubscribe(pf_subscription_t handle);
    /** @brief イベントを該当する購読者へ配信する（主にmonitor_engineが呼ぶ）。@return 常にPF_OK（未初期化時のみエラー） */
    pf_result_t dispatch(const pf_event_t& ev);

private:
    /** @brief 1購読分のスロット。used=falseは空きを表す。 */
    struct subscription {
        bool           used = false;
        pf_state_id_t  id   = 0;
        pf_observer_cb cb   = nullptr;
        void*          ctx  = nullptr;
    };

    /** @brief port の critical_enter を呼ぶ（未登録なら何もしない）。 */
    void lock() const;
    /** @brief lock() に対応する解放処理。 */
    void unlock() const;

    bool         init_ = false;              ///< init() 完了フラグ
    subscription subs_[PF_OBSERVER_MAX]{};   ///< 固定長の購読者テーブル
};

/** @brief port の critical_enter を呼ぶ（未登録なら何もしない）。 */
void observer_dispatcher::lock() const   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
/** @brief lock() に対応する解放処理。 */
void observer_dispatcher::unlock() const { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

/** @brief 全スロットを「未使用」にリセットする。 */
pf_result_t observer_dispatcher::init()
{
    lock();
    for (auto& s : subs_) s.used = false;
    init_ = true;
    unlock();
    return PF_OK;
}

/**
 * @brief 新規購読を登録する。
 *
 * 内部処理: critical区間内でテーブルを先頭から走査し、最初に見つかった
 * 未使用スロットへ登録する。空きが無ければ PF_ERR_NO_SPACE を返す。
 */
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

/**
 * @brief 購読を解除する。
 *
 * 内部処理: ハンドル（スロット番号）の範囲・使用中かを確認したうえで used=false にする。
 * dispatch() は配信対象を事前にスナップショットしてから呼び出すため、
 * このメソッドをコールバック自身の中（＝配信中）から呼んでも安全に解除できる。
 */
pf_result_t observer_dispatcher::unsubscribe(pf_subscription_t handle)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (handle < 0 || static_cast<size_t>(handle) >= PF_OBSERVER_MAX) return PF_ERR_INVALID_ARG;
    if (!subs_[handle].used) return PF_ERR_NOT_FOUND;
    lock();
    subs_[handle].used = false;  // 自分のコールバック内からでも安全（スナップショット配信のため）
    unlock();
    return PF_OK;
}

/**
 * @brief イベントを該当する購読者へ配信する。
 *
 * 内部処理（再入安全のための2段構え）:
 *  1. critical区間内で「配信対象（購読IDが一致 or 全状態購読）のコールバック・文脈」だけを
 *     スタック上の配列へコピーする（スナップショット）。テーブル本体はロックの外に出ない。
 *  2. critical区間を抜けたあと、スナップショットした分だけ順に呼び出す。
 *     この時点でコールバックが subscribe/unsubscribe を呼んでも、走査中の
 *     スナップショット配列自体は変化しないため安全（新規購読は次回配信から反映される）。
 */
pf_result_t observer_dispatcher::dispatch(const pf_event_t& ev)
{
    if (!init_) return PF_ERR_NOT_INIT;

    // 配信対象を critical 内でスナップショット → 解放後に同期呼出（再入安全）
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

/** Observerディスパッチャの唯一のインスタンス（シングルトン・静的記憶域）。 */
observer_dispatcher g_observer;

} // namespace

// ============================================================
// 以下は公開API（extern "C"、詳細は include/printer_fw/pf_observer.h 参照）。
// observer_dispatcher の同名メソッドへ委譲するだけの薄いラッパー。
// ============================================================

/** @copydoc pf_observer_init */
pf_result_t pf_observer_init() { return g_observer.init(); }

/** @copydoc pf_observer_subscribe */
pf_result_t pf_observer_subscribe(pf_state_id_t id, pf_observer_cb cb, void* ctx, pf_subscription_t* out_handle)
{
    return g_observer.subscribe(id, cb, ctx, out_handle);
}

/** @copydoc pf_observer_unsubscribe */
pf_result_t pf_observer_unsubscribe(pf_subscription_t handle) { return g_observer.unsubscribe(handle); }

/** @brief NULLチェックのうえ observer_dispatcher::dispatch へ委譲する。 */
pf_result_t pf_observer_dispatch(const pf_event_t* ev)
{
    if (!ev) return PF_ERR_INVALID_ARG;
    return g_observer.dispatch(*ev);
}
