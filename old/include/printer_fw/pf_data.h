/**
 * @file pf_data.h
 * @brief ① Raw Data Layer ＝ データ辞書（型非依存ストア）。
 *
 * ドライバが取得した生値を、型に依存しないインターフェースで一元管理する。
 * 意味づけはしない（それは ② Abstract State Layer の責務）。全静的確保・O(1)アクセス。
 *
 * ID規約: id は 0..count-1 の稠密値を基本とし、index==id の直接添字でアクセスする。
 *         0xFFFF は予約(PF_STATE_ANY)。有効IDに使ってはならない。
 */
#ifndef PRINTER_FW_PF_DATA_H
#define PRINTER_FW_PF_DATA_H

#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/** データ記述子。モデルが配列で供給し、core がこれに基づき静的確保する。 */
typedef struct {
    pf_data_id_t id;          /**< 一意ID（稠密推奨） */
    pf_type_t    type;        /**< セルの型 */
    uint16_t     elem_size;   /**< BLOB の1要素バイト数（スカラは0でよい） */
    uint16_t     length;      /**< 配列要素数（スカラ/単一は1） */
    uint32_t     change_eps;  /**< 変化とみなす最小差（f32等向け、0=厳密一致） */
    const char*  name;        /**< デバッグ用（任意、NULL可） */
} pf_data_desc_t;

/**
 * @brief 辞書を記述子配列で初期化する（静的確保）。
 * @param desc  記述子配列
 * @param count 要素数（<= PF_DATA_MAX）
 * @return PF_OK / PF_ERR_INVALID_ARG（ID規約違反・重複）/ PF_ERR_NO_SPACE
 */
pf_result_t pf_data_init(const pf_data_desc_t* desc, size_t count);

/* --- 型別便宜API（set はISR安全：内部で critical 保護） --- */
pf_result_t pf_data_set_bool(pf_data_id_t id, bool v);
pf_result_t pf_data_set_u8  (pf_data_id_t id, uint8_t v);
pf_result_t pf_data_set_u16 (pf_data_id_t id, uint16_t v);
pf_result_t pf_data_set_u32 (pf_data_id_t id, uint32_t v);
pf_result_t pf_data_set_i32 (pf_data_id_t id, int32_t v);
pf_result_t pf_data_set_f32 (pf_data_id_t id, float v);
pf_result_t pf_data_set_f64 (pf_data_id_t id, double v);
pf_result_t pf_data_set_enum(pf_data_id_t id, int32_t v);

pf_result_t pf_data_get_bool(pf_data_id_t id, bool* out);
pf_result_t pf_data_get_u8  (pf_data_id_t id, uint8_t* out);
pf_result_t pf_data_get_u16 (pf_data_id_t id, uint16_t* out);
pf_result_t pf_data_get_u32 (pf_data_id_t id, uint32_t* out);
pf_result_t pf_data_get_i32 (pf_data_id_t id, int32_t* out);
pf_result_t pf_data_get_f32 (pf_data_id_t id, float* out);
pf_result_t pf_data_get_f64 (pf_data_id_t id, double* out);
pf_result_t pf_data_get_enum(pf_data_id_t id, int32_t* out);

/* --- 汎用API（構造体・配列・可変長 BLOB） --- */

/** BLOBへ書き込む（src を内部プールへコピー）。size はセル容量以下であること。 */
pf_result_t pf_data_set(pf_data_id_t id, const void* src, uint16_t size);

/** BLOBを読み出す（内部からコピーして dst へ。安全）。 */
pf_result_t pf_data_get(pf_data_id_t id, void* dst, uint16_t size);

/**
 * タグ付き値で取得する。BLOB は out->as.blob.ptr がプール内へのゼロコピー参照。
 * 並行更新があり得る環境では critical 内 / 単一文脈でのみ参照すること。
 */
pf_result_t pf_data_get_value(pf_data_id_t id, pf_value_t* out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_DATA_H */
