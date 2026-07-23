/*
 * rim_list.h - 可変長Listの仕組み（C ABI公開・実体はC++）
 *
 * 要件4(a): 「Listの可変長の仕組み」はC++の仕組みを用いる。
 *   - 実体は C++ の固定容量コンテナ rim::FixedVector<rim_data_entry_item_t, N>
 *     （src/rim_list.cpp）。内部は固定長配列＋サイズで、動的確保をしない。
 *   - C言語からは本ヘッダの extern "C" 不透明ハンドルAPIで、可変長Listとして扱える。
 *
 * 用途例（DataStore §6.4 takeUpdatedValues の戻り、Publisher購読者リスト等）。
 * ハンドルは内部の静的プールから acquire し、使用後 release する（heap不使用）。
 */
#ifndef RIM_LIST_H
#define RIM_LIST_H

#include "rim/rim_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DataEntryItem を要素とする可変長Listの不透明ハンドル */
typedef struct rim_dei_list rim_dei_list_t;

/* 静的プールからList枠を1つ確保（満杯ならNULL）。中身は空で返る。 */
rim_dei_list_t* rim_dei_list_acquire(void);

/* Listをプールへ返却。以後ハンドルは使用不可。 */
void rim_dei_list_release(rim_dei_list_t* list);

/* 末尾追加。容量超過時 RIM_ERR_FULL。 */
rim_result_t rim_dei_list_push(rim_dei_list_t* list, const rim_data_entry_item_t* item);

/* 現在の要素数。 */
size_t rim_dei_list_size(const rim_dei_list_t* list);

/* 固定容量（コンパイル時定数）。 */
size_t rim_dei_list_capacity(const rim_dei_list_t* list);

/*
 * index番目の要素を out へコピー。範囲外は RIM_FALSE を返す。
 * （読み出しはコピー渡し＝生成後不変・単一時点整合性の方針に沿う）
 */
rim_bool rim_dei_list_get(const rim_dei_list_t* list, size_t index, rim_data_entry_item_t* out);

/* 全消去（size=0 に戻す）。容量やプール確保状態は保持。 */
void rim_dei_list_clear(rim_dei_list_t* list);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_LIST_H */
