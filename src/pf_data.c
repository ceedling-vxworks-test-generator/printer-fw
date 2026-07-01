/**
 * @file pf_data.c
 * @brief ① Raw Data Layer（データ辞書）の実装スケルトン。
 *
 * 最小限の動作ロジックを実装（スカラ get/set、BLOB プール、型チェック、ISR安全のための
 * critical 保護）。索引は「稠密IDの直接添字（O(1)）＋疎IDの線形フォールバック」。
 * TODO: 疎IDが多い機種向けの索引表構築、エンディアン変換ヘルパ。
 */
#include "printer_fw/pf_data.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_core.h"   /* pf_core_port() で port を取得 */
#include <string.h>

#define POOL_BYTES ((PF_RAW_POOL_BYTES > 0) ? PF_RAW_POOL_BYTES : 1)

static bool           g_init;
static size_t         g_count;
static pf_data_desc_t g_desc[PF_DATA_MAX];
static pf_value_t     g_val[PF_DATA_MAX];
static uint16_t       g_blob_off[PF_DATA_MAX];
static uint16_t       g_blob_len[PF_DATA_MAX];
static uint8_t        g_pool[POOL_BYTES];

/* --- critical 保護（port があれば利用、無ければ何もしない＝単一文脈想定） --- */
static void lock(void)   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
static void unlock(void) { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

/* ID→スロット。稠密なら直接添字でO(1)、そうでなければ線形探索。 */
static int idx(pf_data_id_t id)
{
    if ((size_t)id < g_count && g_desc[id].id == id) {
        return (int)id;
    }
    for (size_t i = 0; i < g_count; ++i) {
        if (g_desc[i].id == id) return (int)i;
    }
    return -1;
}

pf_result_t pf_data_init(const pf_data_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_DATA_MAX) return PF_ERR_NO_SPACE;

    g_count = 0;
    uint16_t pool_used = 0;

    for (size_t i = 0; i < count; ++i) {
        /* 予約ID・重複チェック */
        if (desc[i].id == (pf_data_id_t)0xFFFFu) return PF_ERR_INVALID_ARG;
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }

        g_desc[i]     = desc[i];
        g_val[i].type = desc[i].type;
        memset(&g_val[i].as, 0, sizeof g_val[i].as);
        g_blob_len[i] = 0;

        if (desc[i].type == PF_TYPE_BLOB) {
            uint32_t cap = (uint32_t)desc[i].elem_size * (uint32_t)desc[i].length;
            if (pool_used + cap > POOL_BYTES) return PF_ERR_NO_SPACE;
            g_blob_off[i] = pool_used;
            pool_used = (uint16_t)(pool_used + cap);
        } else {
            g_blob_off[i] = 0;
        }
    }

    g_count = count;
    g_init  = true;
    return PF_OK;
}

/* --- スカラ set 共通 --- */
static pf_result_t set_scalar(pf_data_id_t id, pf_type_t t, const pf_value_t* v)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (g_desc[s].type != t) return PF_ERR_TYPE_MISMATCH;
    lock();
    g_val[s] = *v;
    g_val[s].type = t;
    unlock();
    return PF_OK;
}

pf_result_t pf_data_set_bool(pf_data_id_t id, bool v)     { pf_value_t x; x.as.b   = v; return set_scalar(id, PF_TYPE_BOOL, &x); }
pf_result_t pf_data_set_u8  (pf_data_id_t id, uint8_t v)  { pf_value_t x; x.as.u8  = v; return set_scalar(id, PF_TYPE_U8,  &x); }
pf_result_t pf_data_set_u16 (pf_data_id_t id, uint16_t v) { pf_value_t x; x.as.u16 = v; return set_scalar(id, PF_TYPE_U16, &x); }
pf_result_t pf_data_set_u32 (pf_data_id_t id, uint32_t v) { pf_value_t x; x.as.u32 = v; return set_scalar(id, PF_TYPE_U32, &x); }
pf_result_t pf_data_set_i32 (pf_data_id_t id, int32_t v)  { pf_value_t x; x.as.i32 = v; return set_scalar(id, PF_TYPE_I32, &x); }
pf_result_t pf_data_set_f32 (pf_data_id_t id, float v)    { pf_value_t x; x.as.f32 = v; return set_scalar(id, PF_TYPE_F32, &x); }
pf_result_t pf_data_set_f64 (pf_data_id_t id, double v)   { pf_value_t x; x.as.f64 = v; return set_scalar(id, PF_TYPE_F64, &x); }
pf_result_t pf_data_set_enum(pf_data_id_t id, int32_t v)  { pf_value_t x; x.as.e   = v; return set_scalar(id, PF_TYPE_ENUM,&x); }

/* --- スカラ get 共通 --- */
static pf_result_t get_scalar(pf_data_id_t id, pf_type_t t, pf_value_t* out)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (g_desc[s].type != t) return PF_ERR_TYPE_MISMATCH;
    lock();
    *out = g_val[s];
    unlock();
    return PF_OK;
}

pf_result_t pf_data_get_bool(pf_data_id_t id, bool* o)     { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_BOOL, &x); if (r==PF_OK&&o) *o = x.as.b;   return r; }
pf_result_t pf_data_get_u8  (pf_data_id_t id, uint8_t* o)  { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_U8,  &x); if (r==PF_OK&&o) *o = x.as.u8;  return r; }
pf_result_t pf_data_get_u16 (pf_data_id_t id, uint16_t* o) { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_U16, &x); if (r==PF_OK&&o) *o = x.as.u16; return r; }
pf_result_t pf_data_get_u32 (pf_data_id_t id, uint32_t* o) { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_U32, &x); if (r==PF_OK&&o) *o = x.as.u32; return r; }
pf_result_t pf_data_get_i32 (pf_data_id_t id, int32_t* o)  { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_I32, &x); if (r==PF_OK&&o) *o = x.as.i32; return r; }
pf_result_t pf_data_get_f32 (pf_data_id_t id, float* o)    { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_F32, &x); if (r==PF_OK&&o) *o = x.as.f32; return r; }
pf_result_t pf_data_get_f64 (pf_data_id_t id, double* o)   { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_F64, &x); if (r==PF_OK&&o) *o = x.as.f64; return r; }
pf_result_t pf_data_get_enum(pf_data_id_t id, int32_t* o)  { pf_value_t x; pf_result_t r = get_scalar(id, PF_TYPE_ENUM,&x); if (r==PF_OK&&o) *o = x.as.e;   return r; }

/* --- BLOB（構造体・配列・可変長） --- */
pf_result_t pf_data_set(pf_data_id_t id, const void* src, uint16_t size)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!src) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (g_desc[s].type != PF_TYPE_BLOB) return PF_ERR_TYPE_MISMATCH;
    uint16_t cap = (uint16_t)(g_desc[s].elem_size * g_desc[s].length);
    if (size > cap) return PF_ERR_OUT_OF_RANGE;
    lock();
    memcpy(&g_pool[g_blob_off[s]], src, size);
    g_blob_len[s] = size;
    unlock();
    return PF_OK;
}

pf_result_t pf_data_get(pf_data_id_t id, void* dst, uint16_t size)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!dst) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (g_desc[s].type != PF_TYPE_BLOB) return PF_ERR_TYPE_MISMATCH;
    if (size < g_blob_len[s]) return PF_ERR_OUT_OF_RANGE;
    lock();
    memcpy(dst, &g_pool[g_blob_off[s]], g_blob_len[s]);
    unlock();
    return PF_OK;
}

pf_result_t pf_data_get_value(pf_data_id_t id, pf_value_t* out)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    lock();
    if (g_desc[s].type == PF_TYPE_BLOB) {
        out->type = PF_TYPE_BLOB;
        out->as.blob.ptr  = &g_pool[g_blob_off[s]];  /* ゼロコピー参照（寿命注意） */
        out->as.blob.size = g_blob_len[s];
    } else {
        *out = g_val[s];
    }
    unlock();
    return PF_OK;
}
