/**
 * @file pf_core.h
 * @brief Core 統括。モデル登録と全レイヤの初期化・1サイクル駆動。
 */
#ifndef PRINTER_FW_PF_CORE_H
#define PRINTER_FW_PF_CORE_H

#include "printer_fw/pf_model.h"
#include "printer_fw/pf_port.h"
#include "printer_fw/pf_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief モデルと port を登録し、全レイヤ（data/state/monitor/observer）を初期化する。
 * @param model 機種記述子（model_xxx_get() の戻り値）
 * @param port  プラットフォーム実装
 * @return PF_OK / PF_ERR_INVALID_ARG / PF_ERR_ALREADY_INIT / 各レイヤのエラー
 */
pf_result_t pf_core_init(const pf_model_t* model, const pf_port_t* port);

/** driver_poll()（あれば）→ pf_monitor_tick() を1回実行する。 */
pf_result_t pf_core_poll_and_tick(void);

/** 登録済み port を返す（内部・port実装が利用）。未初期化なら NULL。 */
const pf_port_t* pf_core_port(void);

/** 終了処理（静的状態をリセット）。 */
void pf_core_deinit(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_CORE_H */
