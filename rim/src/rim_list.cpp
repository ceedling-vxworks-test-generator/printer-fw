/*
 * rim_list.cpp - 可変長Listの実体（C++・要件4(a)）
 *
 * 可変長コンテナ本体 rim::FixedVector は rim/rim_fixed_vector.h に分離済み
 * （1クラス1ファイル方針）。本ファイルは不透明ハンドル rim_dei_list と
 * その静的プール、公開C ABI（rim_list.h）の実装を担う。
 *   - 公開面（rim_list.h）は extern "C" 不透明ハンドル。C側にはC++が一切漏れない。
 */
#include "rim/rim_list.h"
#include "rim/rim_fixed_vector.h"

#include <new>       /* placement new */
#include <cstddef>

/* --- List1本の容量とプール本数（連番enum＋固定長の思想で静的確保） --- */
#ifndef RIM_DEI_LIST_CAPACITY
#define RIM_DEI_LIST_CAPACITY (RIM_ID_COUNT) /* 周期内で全ID最新1件を取り出せる分 */
#endif
#ifndef RIM_DEI_LIST_POOL_SIZE
#define RIM_DEI_LIST_POOL_SIZE 4             /* 同時に使うList枠の本数 */
#endif

/* 不透明ハンドルの実体。C++コンテナ＋使用中フラグを内包する。 */
struct rim_dei_list {
    rim::FixedVector<rim_data_entry_item_t, RIM_DEI_LIST_CAPACITY> vec;
    bool in_use;
    rim_dei_list() : in_use(false) {}
};

/* 静的プール（heap不使用）。 */
static rim_dei_list g_pool[RIM_DEI_LIST_POOL_SIZE];

extern "C" {

rim_dei_list_t* rim_dei_list_acquire(void) {
    for (int i = 0; i < RIM_DEI_LIST_POOL_SIZE; ++i) {
        if (!g_pool[i].in_use) {
            g_pool[i].in_use = true;
            g_pool[i].vec.clear();
            return &g_pool[i];
        }
    }
    return 0; /* 満杯 */
}

void rim_dei_list_release(rim_dei_list_t* list) {
    if (!list) return;
    list->vec.clear();
    list->in_use = false;
}

rim_result_t rim_dei_list_push(rim_dei_list_t* list, const rim_data_entry_item_t* item) {
    if (!list || !item) return RIM_ERR_INVALID_ARG;
    if (!list->vec.push_back(*item)) return RIM_ERR_FULL;
    return RIM_OK;
}

size_t rim_dei_list_size(const rim_dei_list_t* list) {
    return list ? list->vec.size() : 0u;
}

size_t rim_dei_list_capacity(const rim_dei_list_t* list) {
    return list ? list->vec.capacity() : 0u;
}

rim_bool rim_dei_list_get(const rim_dei_list_t* list, size_t index, rim_data_entry_item_t* out) {
    if (!list || !out || index >= list->vec.size()) return RIM_FALSE;
    *out = list->vec.at(index);
    return RIM_TRUE;
}

void rim_dei_list_clear(rim_dei_list_t* list) {
    if (list) list->vec.clear();
}

} /* extern "C" */
