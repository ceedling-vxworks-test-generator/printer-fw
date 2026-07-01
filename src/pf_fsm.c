/**
 * @file pf_fsm.c
 * @brief 主系状態の遷移エンジン実装。許可された遷移のみ通す。
 */
#include "printer_fw/pf_fsm.h"

bool pf_fsm_is_allowed(const pf_fsm_desc_t* fsm, int32_t from, int32_t to)
{
    if (!fsm) return false;
    if (from == to) return true;  /* 自己ループ（変化なし）は常に許可 */
    for (size_t i = 0; i < fsm->transition_count; ++i) {
        if (fsm->transitions[i].from == from && fsm->transitions[i].to == to) {
            return true;
        }
    }
    return false;
}

pf_result_t pf_fsm_validate(const pf_fsm_desc_t* fsm, int32_t from, int32_t to)
{
    if (!fsm) return PF_ERR_INVALID_ARG;
    return pf_fsm_is_allowed(fsm, from, to) ? PF_OK : PF_ERR_INVALID_TRANSITION;
}
