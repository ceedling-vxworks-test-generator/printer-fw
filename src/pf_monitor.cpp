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

class monitor_engine {
public:
    pf_result_t init(const pf_monitor_cfg_t& cfg);
    pf_result_t tick();
    pf_result_t force_emit(pf_state_id_t id);

private:
    pf_time_ms_t          now_ms() const;
    const pf_fsm_desc_t*  find_fsm(pf_state_id_t id) const;
    void                  process_state(size_t slot);

    bool             init_ = false;
    pf_monitor_cfg_t cfg_{};
    int32_t          prev_[PF_STATE_MAX]{};      /* 確定済みの前回値 */
    int32_t          cand_[PF_STATE_MAX]{};      /* デバウンス中の候補値 */
    uint8_t          cand_cnt_[PF_STATE_MAX]{};  /* 候補の連続回数 */
};

pf_time_ms_t monitor_engine::now_ms() const
{
    const pf_port_t* p = pf_core_port();
    return (p && p->time_now) ? p->time_now() : 0u;
}

const pf_fsm_desc_t* monitor_engine::find_fsm(pf_state_id_t id) const
{
    for (size_t i = 0; i < cfg_.fsm_count; ++i) {
        if (cfg_.fsm_list[i].state_id == id) return &cfg_.fsm_list[i];
    }
    return nullptr;
}

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

void monitor_engine::process_state(size_t slot)
{
    pf_state_id_t id = pf_state__id_at(slot);
    int32_t newv = 0;
    if (pf_state_eval(id, &newv) != PF_OK) return;

    int32_t oldv = prev_[slot];

    /* FSM 遷移検証（管理対象のみ）。不許可なら旧値を維持し log。 */
    const pf_fsm_desc_t* fsm = find_fsm(id);
    if (fsm && !pf_fsm_is_allowed(fsm, oldv, newv)) {
        PF_LOGW("monitor: reject invalid transition state=%u %d->%d",
                static_cast<unsigned>(id), static_cast<int>(oldv), static_cast<int>(newv));
        return;
    }

    /* デバウンス（debounce_n>=2 のとき連続一致で確定）。 */
    if (cfg_.debounce_n >= 2) {
        if (newv == cand_[slot]) {
            if (cand_cnt_[slot] < 255) cand_cnt_[slot]++;
        } else {
            cand_[slot]     = newv;
            cand_cnt_[slot] = 1;
        }
        if (cand_cnt_[slot] < cfg_.debounce_n) return;  /* まだ確定しない */
    }

    if (newv == oldv) return;  /* 変化なし → 通知しない */

    /* 確定：状態を書き戻し、イベントを配信 */
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

pf_result_t monitor_engine::tick()
{
    if (!init_) return PF_ERR_NOT_INIT;
    size_t n = pf_state__count();
    for (size_t s = 0; s < n; ++s) {
        process_state(s);
    }
    return PF_OK;
}

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

monitor_engine g_monitor;

} // namespace

pf_result_t pf_monitor_init(const pf_monitor_cfg_t* cfg)
{
    if (!cfg) return PF_ERR_INVALID_ARG;
    return g_monitor.init(*cfg);
}

pf_result_t pf_monitor_tick()                          { return g_monitor.tick(); }
pf_result_t pf_monitor_force_emit(pf_state_id_t id)     { return g_monitor.force_emit(id); }
