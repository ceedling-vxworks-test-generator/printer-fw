/**
 * @file pf_fsm.h
 * @brief 主系ライフサイクル状態の遷移エンジン（許可された遷移のみ通す）。
 *
 * 例: READY → PRINTING → ERROR → RECOVERY。ERROR→PRINTING のような不正遷移を弾く。
 * 未使用（fsm_count=0）ならコストゼロ。
 */
#ifndef PRINTER_FW_PF_FSM_H
#define PRINTER_FW_PF_FSM_H

#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 許可される1遷移（from→to）。 */
typedef struct {
    int32_t from;
    int32_t to;
} pf_fsm_transition_t;

/** ある抽象状態に対する FSM 定義。 */
typedef struct {
    pf_state_id_t              state_id;          /**< この FSM が管理する抽象状態ID */
    int32_t                    initial;           /**< 初期状態値 */
    const pf_fsm_transition_t* transitions;       /**< 許可遷移の配列 */
    size_t                     transition_count;  /**< 遷移数 */
    const char*                name;              /**< デバッグ用（任意） */
} pf_fsm_desc_t;

/** from→to が許可された遷移かを返す（同値 from==to は許可）。 */
bool pf_fsm_is_allowed(const pf_fsm_desc_t* fsm, int32_t from, int32_t to);

/** from→to を検証する。不許可なら PF_ERR_INVALID_TRANSITION。 */
pf_result_t pf_fsm_validate(const pf_fsm_desc_t* fsm, int32_t from, int32_t to);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_FSM_H */
