/**
 * @file pf_data.cpp
 * @brief ① Raw Data Layer（データ辞書）の実装。
 *
 * data_dictionary クラスが辞書の実体（記述子表・値・BLOBプール）を保持する
 * 単一インスタンス（シングルトン・静的記憶域）。全静的確保・稠密ID規約でO(1)。
 * 公開APIは extern "C" のフリー関数として提供し、内部でこのインスタンスへ委譲する。
 */
#include "printer_fw/pf_data.h"
#include "printer_fw/pf_config.h"
#include "printer_fw/pf_core.h"
#include <cstring>

namespace {

constexpr size_t kPoolBytes = (PF_RAW_POOL_BYTES > 0) ? PF_RAW_POOL_BYTES : 1;

class data_dictionary {
public:
    pf_result_t init(const pf_data_desc_t* desc, size_t count);

    pf_result_t set_bool(pf_data_id_t id, bool v)     { pf_value_t x{}; x.as.b   = v; return set_scalar(id, PF_TYPE_BOOL, x); }
    pf_result_t set_u8  (pf_data_id_t id, uint8_t v)  { pf_value_t x{}; x.as.u8  = v; return set_scalar(id, PF_TYPE_U8,  x); }
    pf_result_t set_u16 (pf_data_id_t id, uint16_t v) { pf_value_t x{}; x.as.u16 = v; return set_scalar(id, PF_TYPE_U16, x); }
    pf_result_t set_u32 (pf_data_id_t id, uint32_t v) { pf_value_t x{}; x.as.u32 = v; return set_scalar(id, PF_TYPE_U32, x); }
    pf_result_t set_i32 (pf_data_id_t id, int32_t v)  { pf_value_t x{}; x.as.i32 = v; return set_scalar(id, PF_TYPE_I32, x); }
    pf_result_t set_f32 (pf_data_id_t id, float v)    { pf_value_t x{}; x.as.f32 = v; return set_scalar(id, PF_TYPE_F32, x); }
    pf_result_t set_f64 (pf_data_id_t id, double v)   { pf_value_t x{}; x.as.f64 = v; return set_scalar(id, PF_TYPE_F64, x); }
    pf_result_t set_enum(pf_data_id_t id, int32_t v)  { pf_value_t x{}; x.as.e   = v; return set_scalar(id, PF_TYPE_ENUM, x); }

    pf_result_t get_bool(pf_data_id_t id, bool* out)     { pf_value_t x; auto r = get_scalar(id, PF_TYPE_BOOL, x); if (r==PF_OK&&out) *out=x.as.b;   return r; }
    pf_result_t get_u8  (pf_data_id_t id, uint8_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U8,  x); if (r==PF_OK&&out) *out=x.as.u8;  return r; }
    pf_result_t get_u16 (pf_data_id_t id, uint16_t* out) { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U16, x); if (r==PF_OK&&out) *out=x.as.u16; return r; }
    pf_result_t get_u32 (pf_data_id_t id, uint32_t* out) { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U32, x); if (r==PF_OK&&out) *out=x.as.u32; return r; }
    pf_result_t get_i32 (pf_data_id_t id, int32_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_I32, x); if (r==PF_OK&&out) *out=x.as.i32; return r; }
    pf_result_t get_f32 (pf_data_id_t id, float* out)    { pf_value_t x; auto r = get_scalar(id, PF_TYPE_F32, x); if (r==PF_OK&&out) *out=x.as.f32; return r; }
    pf_result_t get_f64 (pf_data_id_t id, double* out)   { pf_value_t x; auto r = get_scalar(id, PF_TYPE_F64, x); if (r==PF_OK&&out) *out=x.as.f64; return r; }
    pf_result_t get_enum(pf_data_id_t id, int32_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_ENUM, x); if (r==PF_OK&&out) *out=x.as.e;   return r; }

    pf_result_t set(pf_data_id_t id, const void* src, uint16_t size);
    pf_result_t get(pf_data_id_t id, void* dst, uint16_t size);
    pf_result_t get_value(pf_data_id_t id, pf_value_t* out);

private:
    int  index_of(pf_data_id_t id) const;
    void lock() const;
    void unlock() const;
    pf_result_t set_scalar(pf_data_id_t id, pf_type_t t, const pf_value_t& v);
    pf_result_t get_scalar(pf_data_id_t id, pf_type_t t, pf_value_t& out) const;

    bool           init_  = false;
    size_t         count_ = 0;
    pf_data_desc_t desc_[PF_DATA_MAX]{};
    pf_value_t     val_[PF_DATA_MAX]{};
    uint16_t       blob_off_[PF_DATA_MAX]{};
    uint16_t       blob_len_[PF_DATA_MAX]{};
    uint8_t        pool_[kPoolBytes]{};
};

void data_dictionary::lock() const   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
void data_dictionary::unlock() const { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

/* ID規約: 稠密なら直接添字でO(1)。疎IDのみ線形フォールバック。 */
int data_dictionary::index_of(pf_data_id_t id) const
{
    if (static_cast<size_t>(id) < count_ && desc_[id].id == id) {
        return static_cast<int>(id);
    }
    for (size_t i = 0; i < count_; ++i) {
        if (desc_[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

pf_result_t data_dictionary::init(const pf_data_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_DATA_MAX) return PF_ERR_NO_SPACE;

    count_ = 0;
    uint16_t pool_used = 0;

    for (size_t i = 0; i < count; ++i) {
        if (desc[i].id == static_cast<pf_data_id_t>(0xFFFFu)) return PF_ERR_INVALID_ARG;
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }

        desc_[i]     = desc[i];
        val_[i].type = desc[i].type;
        std::memset(&val_[i].as, 0, sizeof val_[i].as);
        blob_len_[i] = 0;

        if (desc[i].type == PF_TYPE_BLOB) {
            uint32_t cap = static_cast<uint32_t>(desc[i].elem_size) * static_cast<uint32_t>(desc[i].length);
            if (pool_used + cap > kPoolBytes) return PF_ERR_NO_SPACE;
            blob_off_[i] = pool_used;
            pool_used = static_cast<uint16_t>(pool_used + cap);
        } else {
            blob_off_[i] = 0;
        }
    }

    count_ = count;
    init_  = true;
    return PF_OK;
}

pf_result_t data_dictionary::set_scalar(pf_data_id_t id, pf_type_t t, const pf_value_t& v)
{
    if (!init_) return PF_ERR_NOT_INIT;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (desc_[s].type != t) return PF_ERR_TYPE_MISMATCH;
    lock();
    val_[s]      = v;
    val_[s].type = t;
    unlock();
    return PF_OK;
}

pf_result_t data_dictionary::get_scalar(pf_data_id_t id, pf_type_t t, pf_value_t& out) const
{
    if (!init_) return PF_ERR_NOT_INIT;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (desc_[s].type != t) return PF_ERR_TYPE_MISMATCH;
    lock();
    out = val_[s];
    unlock();
    return PF_OK;
}

pf_result_t data_dictionary::set(pf_data_id_t id, const void* src, uint16_t size)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!src) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (desc_[s].type != PF_TYPE_BLOB) return PF_ERR_TYPE_MISMATCH;
    uint16_t cap = static_cast<uint16_t>(desc_[s].elem_size * desc_[s].length);
    if (size > cap) return PF_ERR_OUT_OF_RANGE;
    lock();
    std::memcpy(&pool_[blob_off_[s]], src, size);
    blob_len_[s] = size;
    unlock();
    return PF_OK;
}

pf_result_t data_dictionary::get(pf_data_id_t id, void* dst, uint16_t size)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!dst) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    if (desc_[s].type != PF_TYPE_BLOB) return PF_ERR_TYPE_MISMATCH;
    if (size < blob_len_[s]) return PF_ERR_OUT_OF_RANGE;
    lock();
    std::memcpy(dst, &pool_[blob_off_[s]], blob_len_[s]);
    unlock();
    return PF_OK;
}

/* BLOBはプール内へのゼロコピー参照。criticalか単一文脈でのみ参照すること（寿命・整合性は呼出側責務）。 */
pf_result_t data_dictionary::get_value(pf_data_id_t id, pf_value_t* out)
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    lock();
    if (desc_[s].type == PF_TYPE_BLOB) {
        out->type          = PF_TYPE_BLOB;
        out->as.blob.ptr  = &pool_[blob_off_[s]];
        out->as.blob.size = blob_len_[s];
    } else {
        *out = val_[s];
    }
    unlock();
    return PF_OK;
}

data_dictionary g_dictionary;

} // namespace

pf_result_t pf_data_init(const pf_data_desc_t* desc, size_t count) { return g_dictionary.init(desc, count); }

pf_result_t pf_data_set_bool(pf_data_id_t id, bool v)     { return g_dictionary.set_bool(id, v); }
pf_result_t pf_data_set_u8  (pf_data_id_t id, uint8_t v)  { return g_dictionary.set_u8(id, v); }
pf_result_t pf_data_set_u16 (pf_data_id_t id, uint16_t v) { return g_dictionary.set_u16(id, v); }
pf_result_t pf_data_set_u32 (pf_data_id_t id, uint32_t v) { return g_dictionary.set_u32(id, v); }
pf_result_t pf_data_set_i32 (pf_data_id_t id, int32_t v)  { return g_dictionary.set_i32(id, v); }
pf_result_t pf_data_set_f32 (pf_data_id_t id, float v)    { return g_dictionary.set_f32(id, v); }
pf_result_t pf_data_set_f64 (pf_data_id_t id, double v)   { return g_dictionary.set_f64(id, v); }
pf_result_t pf_data_set_enum(pf_data_id_t id, int32_t v)  { return g_dictionary.set_enum(id, v); }

pf_result_t pf_data_get_bool(pf_data_id_t id, bool* out)     { return g_dictionary.get_bool(id, out); }
pf_result_t pf_data_get_u8  (pf_data_id_t id, uint8_t* out)  { return g_dictionary.get_u8(id, out); }
pf_result_t pf_data_get_u16 (pf_data_id_t id, uint16_t* out) { return g_dictionary.get_u16(id, out); }
pf_result_t pf_data_get_u32 (pf_data_id_t id, uint32_t* out) { return g_dictionary.get_u32(id, out); }
pf_result_t pf_data_get_i32 (pf_data_id_t id, int32_t* out)  { return g_dictionary.get_i32(id, out); }
pf_result_t pf_data_get_f32 (pf_data_id_t id, float* out)    { return g_dictionary.get_f32(id, out); }
pf_result_t pf_data_get_f64 (pf_data_id_t id, double* out)   { return g_dictionary.get_f64(id, out); }
pf_result_t pf_data_get_enum(pf_data_id_t id, int32_t* out)  { return g_dictionary.get_enum(id, out); }

pf_result_t pf_data_set(pf_data_id_t id, const void* src, uint16_t size) { return g_dictionary.set(id, src, size); }
pf_result_t pf_data_get(pf_data_id_t id, void* dst, uint16_t size)       { return g_dictionary.get(id, dst, size); }
pf_result_t pf_data_get_value(pf_data_id_t id, pf_value_t* out)          { return g_dictionary.get_value(id, out); }
