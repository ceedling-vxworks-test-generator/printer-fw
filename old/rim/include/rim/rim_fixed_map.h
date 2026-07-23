/*
 * rim_fixed_map.h - 固定容量キー付きコレクション（C++・管理配列の汎用プリミティブ）
 *
 * DataStore が「配列」を管理し、要素の追加/削除/既存更新（キー指定）を行う箇所の実体。
 * FaultRegistry（fault code をキーに raise=追加 / clear=削除 / updated=更新）を汎用化したもの。
 *   - rim::FixedMap<K, V, N> = 固定容量・静的確保のキー付きコレクション（heap不使用）。
 *   - 並列配列（keys_/vals_）＋線形走査。Nは組込みで小さい前提（ハッシュ不使用）。
 *     既存 fault_raise/fault_clear の実装（線形探索＋swap-remove）と同型。
 *   - キーはユニーク（add は重複で false、update/remove はキー不在で false）。
 *   - remove は swap-remove（順序非保持）。走査は key_at/value_at で index 順に。
 *   - 本ヘッダは C++ 専用。公開C ABIからは DataStore 内部でのみ利用され、C側へ漏れない。
 *
 * 「1クラス1ファイル」方針により独立させている（FixedVector とは別invariant＝ユニークキー）。
 */
#ifndef RIM_FIXED_MAP_H
#define RIM_FIXED_MAP_H

#ifndef __cplusplus
#error "rim_fixed_map.h は C++ 専用です（C からは各レイヤの extern \"C\" IFを使用）"
#endif

#include <cstddef>

namespace rim {

/* 固定容量のキー付きコレクション（管理配列の汎用プリミティブ）。 */
template <typename K, typename V, std::size_t N>
class FixedMap {
public:
    FixedMap() : count_(0) {}

    /* キーに対応する値へのポインタ。不在なら nullptr（変更可）。 */
    V* find(const K& key) {
        for (std::size_t i = 0; i < count_; ++i)
            if (keys_[i] == key) return &vals_[i];
        return 0;
    }
    const V* find(const K& key) const {
        for (std::size_t i = 0; i < count_; ++i)
            if (keys_[i] == key) return &vals_[i];
        return 0;
    }

    bool contains(const K& key) const { return find(key) != 0; }

    /* 追加。既存キー or 満杯なら false。 */
    bool add(const K& key, const V& val) {
        if (contains(key)) return false;
        if (count_ >= N) return false;
        keys_[count_] = key;
        vals_[count_] = val;
        ++count_;
        return true;
    }

    /* 既存キーの値を上書き。キー不在なら false。 */
    bool update(const K& key, const V& val) {
        V* p = find(key);
        if (!p) return false;
        *p = val;
        return true;
    }

    /* 追加 or 更新。満杯（新規追加不可）のときのみ false。 */
    bool upsert(const K& key, const V& val) {
        V* p = find(key);
        if (p) { *p = val; return true; }
        return add(key, val);
    }

    /* 削除（swap-remove、順序非保持）。キー不在なら false。 */
    bool remove(const K& key) {
        for (std::size_t i = 0; i < count_; ++i) {
            if (keys_[i] == key) {
                --count_;
                keys_[i] = keys_[count_];
                vals_[i] = vals_[count_];
                return true;
            }
        }
        return false;
    }

    void        clear() { count_ = 0; }
    std::size_t size() const { return count_; }
    std::size_t capacity() const { return N; }

    /* index 順の走査（Snapshot生成等）。i < size() を満たすこと。 */
    const K& key_at(std::size_t i) const { return keys_[i]; }
    const V& value_at(std::size_t i) const { return vals_[i]; }

private:
    K           keys_[N];
    V           vals_[N];
    std::size_t count_;
};

} /* namespace rim */

#endif /* RIM_FIXED_MAP_H */
