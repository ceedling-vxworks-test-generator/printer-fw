/**
 * @file model_sample.h
 * @brief サンプル機種の記述子取得関数。新機種はこれを雛形にする。
 */
#ifndef MODEL_SAMPLE_H
#define MODEL_SAMPLE_H

#include "printer_fw/pf_model.h"
#include "printer_fw/pf_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/** サンプル機種の記述子を返す（core_init に渡す）。 */
const pf_model_t* model_sample_get(void);

/**
 * @brief 音量ドライバのネイティブ型→辞書の正規型(i32)への取り込みグルー例。
 *
 * 機種によってドライバのネイティブ型が異なっても（例: Aドライバ=short, Bドライバ=long）、
 * 辞書 (pf_data) には常に SAMPLE_RAW_VOLUME を i32 として格納する。型の違いを吸収する
 * 変換処理は、このようにモデル側のグルー関数だけに閉じ込め、core・評価器は一切変更しない。
 */
pf_result_t model_sample_volume_ingest_from_short(short native_value);
pf_result_t model_sample_volume_ingest_from_long(long native_value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODEL_SAMPLE_H */
