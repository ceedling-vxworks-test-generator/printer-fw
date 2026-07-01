/**
 * @file pf_result.h
 * @brief 全公開APIの戻り値となる結果コード。PF_OK==0。
 */
#ifndef PRINTER_FW_PF_RESULT_H
#define PRINTER_FW_PF_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif

/** API実行結果。業務的な異常（温度異常等）はこれではなく Abstract State で表す。 */
typedef enum {
    PF_OK = 0,                 /**< 成功 */
    PF_ERR_INVALID_ARG,        /**< NULL / 不正引数 / ID規約違反 */
    PF_ERR_NOT_INIT,           /**< 未初期化で呼ばれた */
    PF_ERR_ALREADY_INIT,       /**< 二重初期化 */
    PF_ERR_NOT_FOUND,          /**< 該当 ID / ハンドルなし */
    PF_ERR_NO_SPACE,           /**< 静的テーブル / プール満杯 */
    PF_ERR_TYPE_MISMATCH,      /**< 期待型と異なる */
    PF_ERR_OUT_OF_RANGE,       /**< サイズ / 範囲外 */
    PF_ERR_INVALID_TRANSITION, /**< FSM で不許可な遷移 */
    PF_ERR_UNSUPPORTED,        /**< 未対応操作 */
    PF_ERR_INTERNAL            /**< 内部不整合（通常 assert 対象） */
} pf_result_t;

/**
 * @brief 結果コードのデバッグ用文字列を返す。
 * @param r 結果コード
 * @return 静的文字列（解放不要）
 */
const char* pf_result_str(pf_result_t r);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_RESULT_H */
