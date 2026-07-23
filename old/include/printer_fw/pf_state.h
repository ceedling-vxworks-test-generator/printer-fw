/**
 * @file pf_state.h
 * @brief ② Abstract State Layer ＝ 評価器レジストリ。
 *
 * 複数の生データから「意味のある状態」を算出する。判定ロジックは評価器（純関数）として
 * モデルが供給し、ここに登録する。状態を増やす＝評価器と記述子を足すだけ（コア無改修）。
 */
#ifndef PRINTER_FW_PF_STATE_H
#define PRINTER_FW_PF_STATE_H

#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 状態評価器。生データ（pf_data_get_* で読む）から状態値を int32 に正規化して出力する。
 * 純関数であること（同じ入力→同じ出力、I/O・時間に依存しない）。
 * @param out_value 出力する状態値（enum/bool/小整数を int32 に正規化）
 * @param ctx       記述子で与えた任意の文脈（しきい値テーブル等。NULL可）
 * @return PF_OK で out_value 有効
 */
typedef pf_result_t (*pf_state_eval_fn)(int32_t* out_value, void* ctx);

/** 抽象状態の記述子。 */
typedef struct {
    pf_state_id_t    id;       /**< 一意ID（稠密推奨） */
    pf_state_eval_fn eval;     /**< 評価器 */
    void*            ctx;      /**< 評価器へ渡す文脈（任意） */
    int32_t          initial;  /**< 初期状態値 */
    const char*      name;     /**< デバッグ用（任意） */
} pf_state_desc_t;

/**
 * @brief 抽象状態を記述子配列で初期化する。
 * @return PF_OK / PF_ERR_INVALID_ARG / PF_ERR_NO_SPACE
 */
pf_result_t pf_state_init(const pf_state_desc_t* desc, size_t count);

/** その場で評価器を実行して現在値を得る。 */
pf_result_t pf_state_eval(pf_state_id_t id, int32_t* out);

/** 直近に Monitor が確定した値を得る（評価器は実行しない）。 */
pf_result_t pf_state_get(pf_state_id_t id, int32_t* out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_STATE_H */
