/**
 * @file pf_fsm.cpp
 * @brief 主系状態の遷移エンジン実装。許可された遷移のみ通す。
 *
 * 保持する状態はなく（渡された記述子だけを見る純関数）、クラス化の対象外。
 */
#include "printer_fw/pf_fsm.h"

/**
 * @brief from→to の遷移が許可されているかを判定する。
 *
 * 内部処理:
 *  1. fsmがNULLなら常に不許可。
 *  2. from==to（自己ループ＝値が変化しない）は常に許可する
 *     （monitor側で「変化なし」として扱われるため、ここで弾く必要がない）。
 *  3. それ以外は許可遷移の一覧(transitions)を先頭から線形探索し、
 *     from/toが完全一致する行があれば許可とする。FSMの遷移数は通常わずか
 *     （機種のライフサイクル数程度）なので線形探索で十分。
 * @return 許可されていれば true。
 */
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

/**
 * @brief from→to の遷移を検証し、結果コードで返す。
 *
 * pf_fsm_is_allowed() を呼ぶだけの薄いラッパー。呼出側（monitor_engine）が
 * bool ではなく pf_result_t で分岐したい場合に使う。
 * @return 許可なら PF_OK、不許可なら PF_ERR_INVALID_TRANSITION、fsmがNULLなら PF_ERR_INVALID_ARG。
 */
pf_result_t pf_fsm_validate(const pf_fsm_desc_t* fsm, int32_t from, int32_t to)
{
    if (!fsm) return PF_ERR_INVALID_ARG;
    return pf_fsm_is_allowed(fsm, from, to) ? PF_OK : PF_ERR_INVALID_TRANSITION;
}
