/**
 * @file pf_core.cpp
 * @brief Core 統括。モデル登録と全レイヤ初期化・1サイクル駆動。
 *
 * core_context クラスが「登録済みモデル・port」を保持する単一インスタンス。
 */
#include "printer_fw/pf_core.h"
#include "printer_fw/pf_data.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_log.h"

namespace {

class core_context {
public:
    pf_result_t      init(const pf_model_t* model, const pf_port_t* port);
    pf_result_t      poll_and_tick();
    const pf_port_t* port() const { return port_; }
    void             deinit();

private:
    bool              init_  = false;
    const pf_model_t* model_ = nullptr;
    const pf_port_t*  port_  = nullptr;
};

pf_result_t core_context::init(const pf_model_t* model, const pf_port_t* port)
{
    if (init_) return PF_ERR_ALREADY_INIT;
    if (!model || !port) return PF_ERR_INVALID_ARG;

    /* port を先に登録（data/observer が critical 取得に使うため） */
    port_  = port;
    model_ = model;

    pf_result_t r;

    r = pf_data_init(model->data_desc, model->data_count);
    if (r != PF_OK) { port_ = nullptr; model_ = nullptr; return r; }

    r = pf_state_init(model->state_desc, model->state_count);
    if (r != PF_OK) { port_ = nullptr; model_ = nullptr; return r; }

    pf_monitor_cfg_t mcfg;
    mcfg.fsm_list   = model->fsm_desc;
    mcfg.fsm_count  = model->fsm_count;
    mcfg.debounce_n = 0;  /* 既定は即時確定。必要ならモデル/アプリで再設定する余地を残す */
    r = pf_monitor_init(&mcfg);
    if (r != PF_OK) { port_ = nullptr; model_ = nullptr; return r; }

    r = pf_observer_init();
    if (r != PF_OK) { port_ = nullptr; model_ = nullptr; return r; }

    if (model->driver_init) {
        r = model->driver_init();
        if (r != PF_OK) { port_ = nullptr; model_ = nullptr; return r; }
    }

    init_ = true;
    PF_LOGI("core: init ok model=%s data=%u state=%u fsm=%u",
            model->name ? model->name : "?",
            static_cast<unsigned>(model->data_count),
            static_cast<unsigned>(model->state_count),
            static_cast<unsigned>(model->fsm_count));
    return PF_OK;
}

pf_result_t core_context::poll_and_tick()
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (model_->driver_poll) {
        pf_result_t r = model_->driver_poll();
        if (r != PF_OK) return r;
    }
    return pf_monitor_tick();
}

void core_context::deinit()
{
    init_  = false;
    model_ = nullptr;
    port_  = nullptr;
}

core_context g_core;

} // namespace

pf_result_t pf_core_init(const pf_model_t* model, const pf_port_t* port) { return g_core.init(model, port); }
pf_result_t pf_core_poll_and_tick()                                     { return g_core.poll_and_tick(); }
const pf_port_t* pf_core_port()                                         { return g_core.port(); }
void pf_core_deinit()                                                   { g_core.deinit(); }
