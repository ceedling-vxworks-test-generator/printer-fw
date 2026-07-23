/**
 * @file pf_monitor.cpp
 * @brief ③ State Monitor Layer（変化検知＋FSM＋デバウンス）の実装。
 *
 * monitor_engine クラスが前回値・デバウンス候補を保持する単一インスタンス。
 * 全状態を再評価し、前回値と比較して変化したものだけを Observer へ dispatch する。
 * FSM管理状態は遷移検証を通す。未使用機能（fsm_count=0 / debounce_n<=1）はゼロコスト。
 */
#include "printer_fw/pf_monitor.h"
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_core.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_log.h"
#include "pf_priv.hpp"

namespace {

/**
 * @brief 変化検知エンジン本体。
 *
 * tick() が呼ばれるたびに全状態を再評価し、
 * 「FSM検証 → デバウンス確定 → 変化判定 → dispatch」の順で処理する。
 */
class monitor_engine {
public:
    /** @brief FSM構成・デバウンス設定を受け取り、前回値を各状態の初期値で初期化する。@return init結果（常にPF_OK） */
    pf_result_t init(const pf_monitor_cfg_t& cfg);
    /** @brief 1サイクル分：全状態を再評価し、変化した状態だけイベントを配信する。@return PF_OK または PF_ERR_NOT_INIT */
    pf_result_t tick();
    /** @brief 指定状態の現在値を、変化の有無に関わらず強制的に1回通知する（初期同期用）。@return dispatch結果 */
    pf_result_t force_emit(pf_state_id_t id);

private:
    /** @brief port の time_now を呼ぶ（port/コールバックが無ければ0を返す）。 */
    pf_time_ms_t          now_ms() const;
    /** @brief 指定状態を管理するFSM定義を cfg_.fsm_list から線形探索する。@return 見つかればそのFSM定義、無ければnullptr */
    const pf_fsm_desc_t*  find_fsm(pf_state_id_t id) const;
    /** @brief 状態1つ分の「評価→FSM検証→デバウンス→変化判定→dispatch」を行う。 */
    void                  process_state(size_t slot);

    bool             init_ = false;              ///< init() 完了フラグ
    pf_monitor_cfg_t cfg_{};                      ///< FSM構成・デバウンス設定（モデルから受け取る）
    int32_t          prev_[PF_STATE_MAX]{};       ///< 確定済みの前回値（変化判定の基準）
    int32_t          cand_[PF_STATE_MAX]{};       ///< デバウンス中の候補値
    uint8_t          cand_cnt_[PF_STATE_MAX]{};   ///< 候補値が連続で一致した回数
};

/** @brief 現在時刻(ms)を取得する。port未登録時は 0 を返す（タイムスタンプの精度が必要ない用途向け）。 */
pf_time_ms_t monitor_engine::now_ms() const
{
    const pf_port_t* p = pf_core_port();
    return (p && p->time_now) ? p->time_now() : 0u;
}

/**
 * @brief 状態IDを管理しているFSM定義を cfg_.fsm_list から探す。
 *
 * FSMの数は通常わずか（機種のライフサイクル数程度）なので線形探索で十分。
 * fsm_count=0（FSM不使用の機種）なら常に nullptr を返す＝呼出元での分岐コストのみでゼロコスト。
 */
const pf_fsm_desc_t* monitor_engine::find_fsm(pf_state_id_t id) const
{
    for (size_t i = 0; i < cfg_.fsm_count; ++i) {
        if (cfg_.fsm_list[i].state_id == id) return &cfg_.fsm_list[i];
    }
    return nullptr;
}

/**
 * @brief Monitorを初期化する。
 *
 * 内部処理: 構成(cfg)を保持したうえで、pf_state 側に登録済みの全状態を走査し、
 * 各状態の「前回値(prev_)」「デバウンス候補(cand_)」を初期値で埋める
 * （これにより初回の tick() で initial→評価値 の差分が正しく検出される）。
 */
pf_result_t monitor_engine::init(const pf_monitor_cfg_t& cfg)
{
    cfg_ = cfg;

    size_t n = pf_state__count();
    for (size_t s = 0; s < n; ++s) {
        prev_[s]     = pf_state__initial_at(s);
        cand_[s]     = prev_[s];
        cand_cnt_[s] = 0;
    }
    init_ = true;
    return PF_OK;
}

/**
 * @brief 1状態分の変化検知処理。
 *
 * 内部処理（上から順に）:
 *  1. 評価器を実行して新値(newv)を得る。評価に失敗したら何もせず終了。
 *  2. その状態がFSM管理対象なら、旧値(oldv)→新値(newv)の遷移が許可されているか検証する。
 *     不許可なら **新値を採用せず**（旧値を維持したまま）WARNログを出して終了する。
 *  3. デバウンスが有効(debounce_n>=2)な場合、新値が前回の候補値と一致するかで連続回数を
 *     数え、規定回数に達するまでは「まだ確定しない」として終了する（チャタリング抑制）。
 *  4. デバウンスを抜けた新値が旧値と同じなら、変化なしとして何もしない（イベントを出さない）。
 *  5. ここまで来たら「変化が確定した」ので、前回値・状態レジストリを更新し、
 *     pf_event_t を組み立てて Observer へ dispatch する（併せてINFOログを出す）。
 */
void monitor_engine::process_state(size_t slot)
{
    pf_state_id_t id = pf_state__id_at(slot);
    int32_t newv = 0;
    if (pf_state_eval(id, &newv) != PF_OK) return;

    int32_t oldv = prev_[slot];

    // FSM 遷移検証（管理対象のみ）。不許可なら旧値を維持し log。
    const pf_fsm_desc_t* fsm = find_fsm(id);
    if (fsm && !pf_fsm_is_allowed(fsm, oldv, newv)) {
        PF_LOGW("monitor: reject invalid transition state=%u %d->%d",
                static_cast<unsigned>(id), static_cast<int>(oldv), static_cast<int>(newv));
        return;
    }

    // デバウンス（debounce_n>=2 のとき連続一致で確定）。
    if (cfg_.debounce_n >= 2) {
        if (newv == cand_[slot]) {
            if (cand_cnt_[slot] < 255) cand_cnt_[slot]++;   // 連続一致をカウント（255で飽和させ暴走を防ぐ）
        } else {
            cand_[slot]     = newv;   // 値が変わったら候補をリセットしてカウントし直す
            cand_cnt_[slot] = 1;
        }
        if (cand_cnt_[slot] < cfg_.debounce_n) return;  // まだ確定しない
    }

    if (newv == oldv) return;  // 変化なし → 通知しない

    // 確定：状態を書き戻し、イベントを配信
    prev_[slot] = newv;
    pf_state__store(id, newv);

    pf_event_t ev;
    ev.state_id  = id;
    ev.old_value = oldv;
    ev.new_value = newv;
    ev.timestamp = now_ms();
    PF_LOGI("monitor: state=%u changed %d->%d (t=%u)",
            static_cast<unsigned>(id), static_cast<int>(oldv), static_cast<int>(newv),
            static_cast<unsigned>(ev.timestamp));
    (void)pf_observer_dispatch(&ev);
}

/** @brief 登録済みの全状態に対して process_state() を順に実行する（1回のtick＝1サイクル）。 */
pf_result_t monitor_engine::tick()
{
    if (!init_) return PF_ERR_NOT_INIT;
    size_t n = pf_state__count();
    for (size_t s = 0; s < n; ++s) {
        process_state(s);
    }
    return PF_OK;
}

/**
 * @brief 指定状態の「現在値」を、変化の有無に関わらず1回だけ強制通知する。
 *
 * 内部処理: pf_state_get() で直近の確定値を取得し、old_value==new_value の
 * イベントを組み立てて dispatch する（主に購読直後の初期同期に使う想定）。
 */
pf_result_t monitor_engine::force_emit(pf_state_id_t id)
{
    if (!init_) return PF_ERR_NOT_INIT;
    int32_t v = 0;
    pf_result_t r = pf_state_get(id, &v);
    if (r != PF_OK) return r;

    pf_event_t ev;
    ev.state_id  = id;
    ev.old_value = v;
    ev.new_value = v;
    ev.timestamp = now_ms();
    return pf_observer_dispatch(&ev);
}

/** Monitorエンジンの唯一のインスタンス（シングルトン・静的記憶域）。 */
monitor_engine g_monitor;

} // namespace

// ============================================================
// 以下は公開API（extern "C"、詳細は include/printer_fw/pf_monitor.h 参照）。
// monitor_engine の同名メソッドへ委譲するだけの薄いラッパー。
// ============================================================

/** @brief NULLチェックのうえ monitor_engine::init へ委譲する。 */
pf_result_t pf_monitor_init(const pf_monitor_cfg_t* cfg)
{
    if (!cfg) return PF_ERR_INVALID_ARG;
    return g_monitor.init(*cfg);
}

/** @copydoc pf_monitor_tick */
pf_result_t pf_monitor_tick()                          { return g_monitor.tick(); }

/** @copydoc pf_monitor_force_emit */
pf_result_t pf_monitor_force_emit(pf_state_id_t id)     { return g_monitor.force_emit(id); }
