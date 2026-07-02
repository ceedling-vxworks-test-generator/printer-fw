/**
 * @file pf_result.cpp
 * @brief 結果コード→文字列。
 */
#include "printer_fw/pf_result.h"

/**
 * @brief pf_result_t を人間可読なデバッグ用文字列へ変換する。
 *
 * 単純な switch による静的文字列テーブル（動的確保なし）。未知の値が来た場合
 * （通常は発生しないが、破損データ等の防御として）は "PF_ERR_?" を返す。
 * @return 結果コードに対応する静的文字列（呼出側で解放不要）。
 */
const char* pf_result_str(pf_result_t r)
{
    switch (r) {
        case PF_OK:                     return "PF_OK";
        case PF_ERR_INVALID_ARG:        return "PF_ERR_INVALID_ARG";
        case PF_ERR_NOT_INIT:           return "PF_ERR_NOT_INIT";
        case PF_ERR_ALREADY_INIT:       return "PF_ERR_ALREADY_INIT";
        case PF_ERR_NOT_FOUND:          return "PF_ERR_NOT_FOUND";
        case PF_ERR_NO_SPACE:           return "PF_ERR_NO_SPACE";
        case PF_ERR_TYPE_MISMATCH:      return "PF_ERR_TYPE_MISMATCH";
        case PF_ERR_OUT_OF_RANGE:       return "PF_ERR_OUT_OF_RANGE";
        case PF_ERR_INVALID_TRANSITION: return "PF_ERR_INVALID_TRANSITION";
        case PF_ERR_UNSUPPORTED:        return "PF_ERR_UNSUPPORTED";
        case PF_ERR_INTERNAL:           return "PF_ERR_INTERNAL";
        default:                        return "PF_ERR_?";
    }
}
