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

/** BLOB用プールの実サイズ。設定0のときも配列宣言が壊れないよう最低1バイト確保する。 */
constexpr size_t kPoolBytes = (PF_RAW_POOL_BYTES > 0) ? PF_RAW_POOL_BYTES : 1;

/**
 * @brief データ辞書の実体。
 *
 * 型タグ付きの値（pf_value_t）を id 単位で保持する、型非依存の静的ストア。
 * スカラは val_[] に、BLOB（構造体・配列・可変長）は pool_[] に実体を持つ。
 */
class data_dictionary {
public:
    /** @brief 記述子配列で辞書を初期化する（静的確保・稠密ID検証）。@return init結果 */
    pf_result_t init(const pf_data_desc_t* desc, size_t count);

    /** 型別 setter 群。共通処理は set_scalar() に委譲し、型不一致は PF_ERR_TYPE_MISMATCH を返す。 */
    pf_result_t set_bool(pf_data_id_t id, bool v)     { pf_value_t x{}; x.as.b   = v; return set_scalar(id, PF_TYPE_BOOL, x); }
    pf_result_t set_u8  (pf_data_id_t id, uint8_t v)  { pf_value_t x{}; x.as.u8  = v; return set_scalar(id, PF_TYPE_U8,  x); }
    pf_result_t set_u16 (pf_data_id_t id, uint16_t v) { pf_value_t x{}; x.as.u16 = v; return set_scalar(id, PF_TYPE_U16, x); }
    pf_result_t set_u32 (pf_data_id_t id, uint32_t v) { pf_value_t x{}; x.as.u32 = v; return set_scalar(id, PF_TYPE_U32, x); }
    pf_result_t set_i32 (pf_data_id_t id, int32_t v)  { pf_value_t x{}; x.as.i32 = v; return set_scalar(id, PF_TYPE_I32, x); }
    pf_result_t set_f32 (pf_data_id_t id, float v)    { pf_value_t x{}; x.as.f32 = v; return set_scalar(id, PF_TYPE_F32, x); }
    pf_result_t set_f64 (pf_data_id_t id, double v)   { pf_value_t x{}; x.as.f64 = v; return set_scalar(id, PF_TYPE_F64, x); }
    pf_result_t set_enum(pf_data_id_t id, int32_t v)  { pf_value_t x{}; x.as.e   = v; return set_scalar(id, PF_TYPE_ENUM, x); }

    /** 型別 getter 群。共通処理は get_scalar() に委譲する。out が NULL でなければ値を書き戻す。 */
    pf_result_t get_bool(pf_data_id_t id, bool* out)     { pf_value_t x; auto r = get_scalar(id, PF_TYPE_BOOL, x); if (r==PF_OK&&out) *out=x.as.b;   return r; }
    pf_result_t get_u8  (pf_data_id_t id, uint8_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U8,  x); if (r==PF_OK&&out) *out=x.as.u8;  return r; }
    pf_result_t get_u16 (pf_data_id_t id, uint16_t* out) { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U16, x); if (r==PF_OK&&out) *out=x.as.u16; return r; }
    pf_result_t get_u32 (pf_data_id_t id, uint32_t* out) { pf_value_t x; auto r = get_scalar(id, PF_TYPE_U32, x); if (r==PF_OK&&out) *out=x.as.u32; return r; }
    pf_result_t get_i32 (pf_data_id_t id, int32_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_I32, x); if (r==PF_OK&&out) *out=x.as.i32; return r; }
    pf_result_t get_f32 (pf_data_id_t id, float* out)    { pf_value_t x; auto r = get_scalar(id, PF_TYPE_F32, x); if (r==PF_OK&&out) *out=x.as.f32; return r; }
    pf_result_t get_f64 (pf_data_id_t id, double* out)   { pf_value_t x; auto r = get_scalar(id, PF_TYPE_F64, x); if (r==PF_OK&&out) *out=x.as.f64; return r; }
    pf_result_t get_enum(pf_data_id_t id, int32_t* out)  { pf_value_t x; auto r = get_scalar(id, PF_TYPE_ENUM, x); if (r==PF_OK&&out) *out=x.as.e;   return r; }

    /** @brief BLOB（構造体・配列・可変長）を src からプールへコピーする。@return set結果 */
    pf_result_t set(pf_data_id_t id, const void* src, uint16_t size);
    /** @brief BLOBをプールから dst へコピーする（安全なコピー取得）。@return get結果 */
    pf_result_t get(pf_data_id_t id, void* dst, uint16_t size);
    /** @brief 型を問わずタグ付き値として取得する。BLOBはゼロコピー参照になる。@return get結果 */
    pf_result_t get_value(pf_data_id_t id, pf_value_t* out);

private:
    /** @brief id からスロット番号を引く。稠密IDならO(1)、それ以外はO(n)線形探索。@return スロット番号、未検出は-1 */
    int  index_of(pf_data_id_t id) const;
    /** @brief port経由でクリティカルセクションに入る（未登録portなら何もしない）。 */
    void lock() const;
    /** @brief lock() に対応する解放処理。 */
    void unlock() const;
    /** @brief 型別setterが共有する内部実装（初期化チェック・ID検索・型チェック・書込を一括で行う）。 */
    pf_result_t set_scalar(pf_data_id_t id, pf_type_t t, const pf_value_t& v);
    /** @brief 型別getterが共有する内部実装（初期化チェック・ID検索・型チェック・読取を一括で行う）。 */
    pf_result_t get_scalar(pf_data_id_t id, pf_type_t t, pf_value_t& out) const;

    bool           init_  = false;               ///< init() 完了フラグ
    size_t         count_ = 0;                   ///< 登録済みデータ数
    pf_data_desc_t desc_[PF_DATA_MAX]{};          ///< id・型・要素数などの記述子表
    pf_value_t     val_[PF_DATA_MAX]{};           ///< スカラ値の実体（型タグ付き共用体）
    uint16_t       blob_off_[PF_DATA_MAX]{};      ///< BLOBのプール内オフセット
    uint16_t       blob_len_[PF_DATA_MAX]{};      ///< BLOBの実際の書き込み済みバイト数
    uint8_t        pool_[kPoolBytes]{};           ///< BLOB実体を詰める固定サイズプール
};

/** @brief port の critical_enter を呼ぶ（port/コールバックが無ければ何もしない）。 */
void data_dictionary::lock() const   { const pf_port_t* p = pf_core_port(); if (p && p->critical_enter) p->critical_enter(); }
/** @brief port の critical_exit を呼ぶ（port/コールバックが無ければ何もしない）。 */
void data_dictionary::unlock() const { const pf_port_t* p = pf_core_port(); if (p && p->critical_exit)  p->critical_exit();  }

/**
 * @brief id をスロット番号へ変換する（ID規約: 0..count-1 の稠密値を基本とする）。
 *
 * 内部処理:
 *  1. まず「id自身がそのままスロット番号」という前提で直接添字を試す（O(1)）。
 *     稠密なIDであれば、この分岐だけでヒットして即座に返る。
 *  2. 1で外れた場合（疎なID構成の機種向け）は、記述子表を先頭から線形に探す（O(n)フォールバック）。
 * @return 見つかったスロット番号。見つからなければ -1。
 */
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

/**
 * @brief 記述子配列で辞書を初期化する。
 *
 * 内部処理:
 *  1. 引数チェック（NULL・上限超過）。
 *  2. 各記述子について、予約ID(0xFFFF)でないこと・IDが重複していないことを検証する
 *     （不正なら初期化を中断してエラーを返す。part-wayで失敗した場合、count_ は
 *     まだ確定していないため以後のアクセスは「空の辞書」として扱われる）。
 *  3. 記述子・初期値（型タグのみ設定・値はゼロ）をコピーする。
 *  4. 型が BLOB の場合のみ、固定プール内の領域をオフセット方式で予約する
 *     （記述子の登場順に隙間なく詰める。プール容量を超えたら NO_SPACE で失敗）。
 *  5. 全件処理できたら count_/init_ を確定する。
 */
pf_result_t data_dictionary::init(const pf_data_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_DATA_MAX) return PF_ERR_NO_SPACE;

    count_ = 0;
    uint16_t pool_used = 0;

    for (size_t i = 0; i < count; ++i) {
        // 予約IDチェック: 0xFFFF は「全状態購読」の番兵(PF_STATE_ANY)のため、有効なデータIDには使えない。
        if (desc[i].id == static_cast<pf_data_id_t>(0xFFFFu)) return PF_ERR_INVALID_ARG;
        // 重複IDチェック: これまで登録した分と比較する（O(n^2)だが初期化時のみなので許容）。
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }

        desc_[i]     = desc[i];
        val_[i].type = desc[i].type;
        std::memset(&val_[i].as, 0, sizeof val_[i].as);   // 未設定値を0クリアしておく
        blob_len_[i] = 0;

        if (desc[i].type == PF_TYPE_BLOB) {
            // BLOBはプール内の連続領域をオフセット方式で予約する（要素サイズ×要素数）
            uint32_t cap = static_cast<uint32_t>(desc[i].elem_size) * static_cast<uint32_t>(desc[i].length);
            if (pool_used + cap > kPoolBytes) return PF_ERR_NO_SPACE;
            blob_off_[i] = pool_used;
            pool_used = static_cast<uint16_t>(pool_used + cap);
        } else {
            blob_off_[i] = 0;   // スカラはプール未使用
        }
    }

    count_ = count;
    init_  = true;
    return PF_OK;
}

/**
 * @brief スカラ値を書き込む（型別setterの共通実装）。
 *
 * 内部処理: 初期化済みか確認 → id からスロットを引く → 記述子の型と一致するか確認
 * → critical区間で値本体を書き換える（型タグも上書きし整合を保つ）。
 */
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

/**
 * @brief スカラ値を読み出す（型別getterの共通実装）。
 *
 * 内部処理: set_scalar() と対称の手順で、critical区間内で値をコピーして返す。
 */
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

/**
 * @brief BLOBデータを書き込む。
 *
 * 内部処理: 型がBLOBであることを確認 → 記述子から求めた容量(elem_size×length)を
 * 超えていないか検証 → critical区間でプール内の該当オフセットへ memcpy し、
 * 実際に書き込んだ長さ(blob_len_)を更新する。
 */
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

/**
 * @brief BLOBデータを読み出す（呼出側バッファへコピーする安全な取得）。
 *
 * 内部処理: 型がBLOBであることを確認 → 呼出側バッファ(size)が実データ長以上か検証
 * → critical区間でプールから呼出側バッファへ memcpy する。
 */
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

/**
 * @brief 型を問わずタグ付き値(pf_value_t)として取得する。
 *
 * 内部処理: BLOBの場合はコピーせず「プール内位置＋長さ」への参照（ゼロコピー）を
 * 返す（criticalか単一文脈でのみ有効。呼出側は寿命・整合性に注意すること）。
 * スカラの場合は val_ の実体をそのままコピーして返す。
 */
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

/** データ辞書の唯一のインスタンス（シングルトン・静的記憶域）。 */
data_dictionary g_dictionary;

} // namespace

// ============================================================
// 以下は公開API（extern "C"、詳細は include/printer_fw/pf_data.h 参照）。
// data_dictionary の同名メソッドへ委譲するだけの薄いラッパー。
// ============================================================

/** @copydoc pf_data_init */
pf_result_t pf_data_init(const pf_data_desc_t* desc, size_t count) { return g_dictionary.init(desc, count); }

/** @brief 型別 setter 群。data_dictionary の同名メソッドへ委譲する（ISR安全：内部で critical 保護）。 */
pf_result_t pf_data_set_bool(pf_data_id_t id, bool v)     { return g_dictionary.set_bool(id, v); }
pf_result_t pf_data_set_u8  (pf_data_id_t id, uint8_t v)  { return g_dictionary.set_u8(id, v); }
pf_result_t pf_data_set_u16 (pf_data_id_t id, uint16_t v) { return g_dictionary.set_u16(id, v); }
pf_result_t pf_data_set_u32 (pf_data_id_t id, uint32_t v) { return g_dictionary.set_u32(id, v); }
pf_result_t pf_data_set_i32 (pf_data_id_t id, int32_t v)  { return g_dictionary.set_i32(id, v); }
pf_result_t pf_data_set_f32 (pf_data_id_t id, float v)    { return g_dictionary.set_f32(id, v); }
pf_result_t pf_data_set_f64 (pf_data_id_t id, double v)   { return g_dictionary.set_f64(id, v); }
pf_result_t pf_data_set_enum(pf_data_id_t id, int32_t v)  { return g_dictionary.set_enum(id, v); }

/** @brief 型別 getter 群。data_dictionary の同名メソッドへ委譲する。 */
pf_result_t pf_data_get_bool(pf_data_id_t id, bool* out)     { return g_dictionary.get_bool(id, out); }
pf_result_t pf_data_get_u8  (pf_data_id_t id, uint8_t* out)  { return g_dictionary.get_u8(id, out); }
pf_result_t pf_data_get_u16 (pf_data_id_t id, uint16_t* out) { return g_dictionary.get_u16(id, out); }
pf_result_t pf_data_get_u32 (pf_data_id_t id, uint32_t* out) { return g_dictionary.get_u32(id, out); }
pf_result_t pf_data_get_i32 (pf_data_id_t id, int32_t* out)  { return g_dictionary.get_i32(id, out); }
pf_result_t pf_data_get_f32 (pf_data_id_t id, float* out)    { return g_dictionary.get_f32(id, out); }
pf_result_t pf_data_get_f64 (pf_data_id_t id, double* out)   { return g_dictionary.get_f64(id, out); }
pf_result_t pf_data_get_enum(pf_data_id_t id, int32_t* out)  { return g_dictionary.get_enum(id, out); }

// --- 汎用API（構造体・配列・可変長 BLOB） ---

/** @copydoc pf_data_set */
pf_result_t pf_data_set(pf_data_id_t id, const void* src, uint16_t size) { return g_dictionary.set(id, src, size); }

/** @copydoc pf_data_get */
pf_result_t pf_data_get(pf_data_id_t id, void* dst, uint16_t size)       { return g_dictionary.get(id, dst, size); }

/** @copydoc pf_data_get_value */
pf_result_t pf_data_get_value(pf_data_id_t id, pf_value_t* out)          { return g_dictionary.get_value(id, out); }
