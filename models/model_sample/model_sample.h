/**
 * @file model_sample.h
 * @brief サンプル機種の記述子取得関数。新機種はこれを雛形にする。
 */
#ifndef MODEL_SAMPLE_H
#define MODEL_SAMPLE_H

#include "printer_fw/pf_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/** サンプル機種の記述子を返す（core_init に渡す）。 */
const pf_model_t* model_sample_get(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODEL_SAMPLE_H */
