/**
 * @file pf_core.c
 * @brief Core 統括。モデル登録と全レイヤ初期化・1サイクル駆動。
 */
#include "printer_fw/pf_core.h"
#include "printer_fw/pf_data.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_log.h"

static bool             g_init;
static const pf_model_t* g_model;
static const pf_port_t*  g_port;

const pf_port_t* pf_core_port(void)
{
    return g_port;
}

pf_result_t pf_core_init(const pf_model_t* model, const pf_port_t* port)
{
    if (g_init) return PF_ERR_ALREADY_INIT;
    if (!model || !port) return PF_ERR_INVALID_ARG;

    /* port を先に登録（data/observer が critical 取得に使うため） */
    g_port  = port;
    g_model = model;

    pf_result_t r;

    r = pf_data_init(model->data_desc, model->data_count);
    if (r != PF_OK) { g_port = NULL; g_model = NULL; return r; }

    r = pf_state_init(model->state_desc, model->state_count);
    if (r != PF_OK) { g_port = NULL; g_model = NULL; return r; }

    pf_monitor_cfg_t mcfg;
    mcfg.fsm_list   = model->fsm_desc;
    mcfg.fsm_count  = model->fsm_count;
    mcfg.debounce_n = 0;  /* 既定は即時確定。必要ならモデル/アプリで再設定する余地を残す */
    r = pf_monitor_init(&mcfg);
    if (r != PF_OK) { g_port = NULL; g_model = NULL; return r; }

    r = pf_observer_init();
    if (r != PF_OK) { g_port = NULL; g_model = NULL; return r; }

    if (model->driver_init) {
        r = model->driver_init();
        if (r != PF_OK) { g_port = NULL; g_model = NULL; return r; }
    }

    g_init = true;
    PF_LOGI("core: init ok model=%s data=%u state=%u fsm=%u",
            model->name ? model->name : "?",
            (unsigned)model->data_count, (unsigned)model->state_count,
            (unsigned)model->fsm_count);
    return PF_OK;
}

pf_result_t pf_core_poll_and_tick(void)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (g_model->driver_poll) {
        pf_result_t r = g_model->driver_poll();
        if (r != PF_OK) return r;
    }
    return pf_monitor_tick();
}

void pf_core_deinit(void)
{
    g_init  = false;
    g_model = NULL;
    g_port  = NULL;
}
