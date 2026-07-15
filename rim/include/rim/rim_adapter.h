/*
 * rim_adapter.h - L1 RIM_AdapterLayer 公開IF（C ABI）
 *
 * 要件4(b): 「AdapterLayerでデータを受け取る箇所の型の自由さ」はC++の仕組みで実現する。
 *   - 受理点は入力元ごとに native な型が異なる（i32 / double / u32 ...）。
 *   - C言語からは、型ごとの push 関数として自然に呼べる（Cにはオーバーロードが無いため
 *     関数名で型を表す）。内部（src/rim_adapter.cpp）ではC++テンプレートで
 *     これらを1つの正規化パスへ集約し、「型の自由さ」を吸収する。
 *   - 正規化後は rim_data_value_t（C表現）へ落として後続層へ渡す。
 *
 * RawDataInput詳細 §9: push(RawData)->Result。ここでは型別の push を提供する。
 */
#ifndef RIM_ADAPTER_H
#define RIM_ADAPTER_H

#include "rim/rim_types.h"

#ifdef __cplusplus
extern "C" {
#endif

rim_result_t rim_adapter_init(void);
void         rim_adapter_shutdown(void);

/*
 * 型の自由な受理点（native型ごとのpush）。
 * 内部でRule選択→convert→性質判別(InputClassifier)→CentralInputPortの3種postを行う。
 * ctx はNULL可（未使用時）。
 */
rim_result_t rim_adapter_push_i32(rim_data_id_t id, int32_t raw,
                                  const rim_data_context_t* ctx);
rim_result_t rim_adapter_push_f  (rim_data_id_t id, double raw,
                                  const rim_data_context_t* ctx);
rim_result_t rim_adapter_push_u32(rim_data_id_t id, uint32_t raw,
                                  const rim_data_context_t* ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_ADAPTER_H */
