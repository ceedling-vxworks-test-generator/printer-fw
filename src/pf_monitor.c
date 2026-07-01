/**
 * @file pf_monitor.c
 * @brief ③ State Monitor Layer（変化検知＋FSM＋デバウンス）の実装スケルトン。
 *
 * 全状態を再評価し、前回値と比較して変化したものだけを Observer へ dispatch する。
 * FSM管理状態は遷移検証を通す。未使用機能（fsm_count=0 / debounce_n<=1）はゼロコスト。
 */
#include "printer_fw/pf_monitor.h"
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_core.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_log.h"
#include "pf_priv.h"

static bool             g_init;
static pf_monitor_cfg_t g_cfg;
static int32_t          g_prev[PF_STATE_MAX];     /* 確定済みの前回値 */
static int32_t          g_cand[PF_STATE_MAX];     /* デバウンス中の候補値 */
static uint8_t          g_cand_cnt[PF_STATE_MAX]; /* 候補の連続回数 */

static pf_time_ms_t now_ms(void)
{
    const pf_port_t* p = pf_core_port();
    return (p && p->time_now) ? p->time_now() : 0u;
}

/* state_id を管理する FSM を探す（無ければ NULL）。 */
static const pf_fsm_desc_t* find_fsm(pf_state_id_t id)
{
    for (size_t i = 0; i < g_cfg.fsm_count; ++i) {
        if (g_cfg.fsm_list[i].state_id == id) return &g_cfg.fsm_list[i];
    }
    return NULL;
}

pf_result_t pf_monitor_init(const pf_monitor_cfg_t* cfg)
{
    if (!cfg) return PF_ERR_INVALID_ARG;
    g_cfg = *cfg;

    size_t n = pf_state__count();
    for (size_t s = 0; s < n; ++s) {
        g_prev[s]     = pf_state__initial_at(s);
        g_cand[s]     = g_prev[s];
        g_cand_cnt[s] = 0;
    }
    g_init = true;
    return PF_OK;
}

/* 1状態の変化処理（確定したら dispatch）。 */
static void process_state(size_t slot)
{
    pf_state_id_t id = pf_state__id_at(slot);
    int32_t newv = 0;
    if (pf_state_eval(id, &newv) != PF_OK) return;

    int32_t oldv = g_prev[slot];

    /* FSM 遷移検証（管理対象のみ）。不許可なら旧値を維持し log。 */
    const pf_fsm_desc_t* fsm = find_fsm(id);
    if (fsm && !pf_fsm_is_allowed(fsm, oldv, newv)) {
        PF_LOGW("monitor: reject invalid transition state=%u %d->%d",
                (unsigned)id, (int)oldv, (int)newv);
        return;
    }

    /* デバウンス（debounce_n>=2 のとき連続一致で確定）。 */
    if (g_cfg.debounce_n >= 2) {
        if (newv == g_cand[slot]) {
            if (g_cand_cnt[slot] < 255) g_cand_cnt[slot]++;
        } else {
            g_cand[slot] = newv;
            g_cand_cnt[slot] = 1;
        }
        if (g_cand_cnt[slot] < g_cfg.debounce_n) return;  /* まだ確定しない */
    }

    if (newv == oldv) return;  /* 変化なし → 通知しない */

    /* 確定：状態を書き戻し、イベントを配信 */
    g_prev[slot] = newv;
    pf_state__store(id, newv);

    pf_event_t ev;
    ev.state_id  = id;
    ev.old_value = oldv;
    ev.new_value = newv;
    ev.timestamp = now_ms();
    PF_LOGI("monitor: state=%u changed %d->%d (t=%u)",
            (unsigned)id, (int)oldv, (int)newv, (unsigned)ev.timestamp);
    (void)pf_observer_dispatch(&ev);
}

pf_result_t pf_monitor_tick(void)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    size_t n = pf_state__count();
    for (size_t s = 0; s < n; ++s) {
        process_state(s);
    }
    return PF_OK;
}

pf_result_t pf_monitor_force_emit(pf_state_id_t id)
{
    if (!g_init) return PF_ERR_NOT_INIT;
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
