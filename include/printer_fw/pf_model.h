/**
 * @file pf_model.h
 * @brief 機種依存のすべてを集約する記述子。1機種 = 1つの pf_model_t。
 *
 * 各機種は data/state/fsm の記述子表とドライバグルーを実装し、
 * `const pf_model_t* model_xxx_get(void)` を1つ提供する。core は特定機種を知らない（依存性逆転）。
 */
#ifndef PRINTER_FW_PF_MODEL_H
#define PRINTER_FW_PF_MODEL_H

#include "printer_fw/pf_data.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 機種記述子。core はこれを受け取り各レイヤを構成する。 */
typedef struct {
    const char*            name;        /**< 機種名（デバッグ用） */

    const pf_data_desc_t*  data_desc;   /**< データ辞書の記述子表 */
    size_t                 data_count;

    const pf_state_desc_t* state_desc;  /**< 抽象状態の記述子表 */
    size_t                 state_count;

    const pf_fsm_desc_t*   fsm_desc;    /**< FSM定義（任意、NULL可） */
    size_t                 fsm_count;

    pf_result_t          (*driver_init)(void); /**< HW初期化（任意、NULL可） */
    pf_result_t          (*driver_poll)(void); /**< HW→辞書へ取り込み（任意、NULL可） */
} pf_model_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_MODEL_H */
