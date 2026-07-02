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

/**
 * @brief Core統括の実体。
 *
 * 「どのモデル・どのportで動いているか」だけを保持し、実際の初期化処理は
 * 各レイヤ（pf_data/pf_state/pf_monitor/pf_observer）の公開APIへ委譲する。
 */
class core_context {
public:
    /** @brief モデル・portを登録し、全レイヤを順に初期化する。@return init結果 */
    pf_result_t      init(const pf_model_t* model, const pf_port_t* port);
    /** @brief driver_poll()（あれば）→ 変化検知(tick) を1回実行する。@return poll_and_tick結果 */
    pf_result_t      poll_and_tick();
    /** @brief 登録済みport（未登録ならnullptr）を返す。他レイヤが critical/time/log の取得に使う。 */
    const pf_port_t* port() const { return port_; }
    /** @brief 内部状態をリセットする（再初期化を可能にする）。 */
    void             deinit();

private:
    bool              init_  = false;   ///< init() 完了フラグ
    const pf_model_t* model_ = nullptr; ///< 登録済みモデル記述子（機種依存の集合）
    const pf_port_t*  port_  = nullptr; ///< 登録済みプラットフォーム実装
};

/**
 * @brief モデル・portを登録し、全レイヤを初期化する。
 *
 * 内部処理（上から順に）:
 *  1. 二重初期化・NULL引数を弾く。
 *  2. port_/model_ を先に確定させる（下位レイヤのcritical取得等が pf_core_port() 経由で
 *     すぐに使えるようにするため、data/state/monitor/observerの初期化より先に行う）。
 *  3. pf_data → pf_state → pf_monitor → pf_observer の順に各レイヤを初期化する。
 *     いずれかが失敗したら、その時点で port_/model_ を未登録状態へ戻して中断する
 *     （中途半端な初期化のまま「登録済み」に見えないようにするため）。
 *  4. モデルが driver_init を持っていれば、最後にHW初期化を呼ぶ。
 *  5. 全て成功したら init_=true にし、構成概要をINFOログへ出力する。
 */
pf_result_t core_context::init(const pf_model_t* model, const pf_port_t* port)
{
    if (init_) return PF_ERR_ALREADY_INIT;
    if (!model || !port) return PF_ERR_INVALID_ARG;

    // port を先に登録（data/observer が critical 取得に使うため）
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
    mcfg.debounce_n = 0;  // 既定は即時確定。必要ならモデル/アプリで再設定する余地を残す
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

/**
 * @brief 1サイクル分の駆動処理。
 *
 * 内部処理: モデルにdriver_pollが登録されていればまずそれを呼んでHW値を辞書へ
 * 取り込ませ、その後 pf_monitor_tick() で変化検知・通知までを行う。
 * driver_poll が失敗したら、tick は実行せずそのままエラーを返す。
 */
pf_result_t core_context::poll_and_tick()
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (model_->driver_poll) {
        pf_result_t r = model_->driver_poll();
        if (r != PF_OK) return r;
    }
    return pf_monitor_tick();
}

/** @brief フラグとポインタをクリアするだけ（各レイヤ内部の静的テーブルは保持されたままになる）。 */
void core_context::deinit()
{
    init_  = false;
    model_ = nullptr;
    port_  = nullptr;
}

/** Core統括の唯一のインスタンス（シングルトン・静的記憶域）。 */
core_context g_core;

} // namespace

// ============================================================
// 以下は公開API（extern "C"、詳細は include/printer_fw/pf_core.h 参照）。
// core_context の同名メソッドへ委譲するだけの薄いラッパー。
// ============================================================
pf_result_t pf_core_init(const pf_model_t* model, const pf_port_t* port) { return g_core.init(model, port); }
pf_result_t pf_core_poll_and_tick()                                     { return g_core.poll_and_tick(); }
const pf_port_t* pf_core_port()                                         { return g_core.port(); }
void pf_core_deinit()                                                   { g_core.deinit(); }
