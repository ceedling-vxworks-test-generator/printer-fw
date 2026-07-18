#pragma once

//
// FixedMap<K,V,N> - 固定容量キー付きコレクション(管理配列の汎用プリミティブ)。
// rim/rim_fixed_map.h からの移植。並列配列(keys/vals)＋線形走査。
// FaultRegistry(fault code をキーに add/remove/update)の実体。
//   - キーはユニーク。Add は重複で false、Update/Remove はキー不在で false。
//   - Remove は swap-remove(順序非保持)。走査は KeyAt/ValueAt で index 順に。
//

#include <cstddef>

namespace rim
{

template <typename K, typename V, std::size_t N>
class FixedMap
{
public:

    FixedMap()
        : count_(0)
    {
    }

    V* Find(const K& key)
    {
        for (std::size_t i = 0; i < count_; ++i)
            if (keys_[i] == key) return &vals_[i];
        return nullptr;
    }

    const V* Find(const K& key) const
    {
        for (std::size_t i = 0; i < count_; ++i)
            if (keys_[i] == key) return &vals_[i];
        return nullptr;
    }

    bool Contains(const K& key) const { return Find(key) != nullptr; }

    // 追加。既存キー or 満杯なら false。
    bool Add(const K& key, const V& val)
    {
        if (Contains(key)) return false;
        if (count_ >= N) return false;
        keys_[count_] = key;
        vals_[count_] = val;
        ++count_;
        return true;
    }

    // 既存キーの値を上書き。キー不在なら false。
    bool Update(const K& key, const V& val)
    {
        V* p = Find(key);
        if (!p) return false;
        *p = val;
        return true;
    }

    // 追加 or 更新。満杯(新規追加不可)のときのみ false。
    bool Upsert(const K& key, const V& val)
    {
        V* p = Find(key);
        if (p) { *p = val; return true; }
        return Add(key, val);
    }

    // 削除(swap-remove、順序非保持)。キー不在なら false。
    bool Remove(const K& key)
    {
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

    void        Clear() { count_ = 0; }
    std::size_t Size()  const { return count_; }
    std::size_t Capacity() const { return N; }

    const K& KeyAt(std::size_t i)   const { return keys_[i]; }
    const V& ValueAt(std::size_t i) const { return vals_[i]; }

private:

    K           keys_[N];
    V           vals_[N];
    std::size_t count_;
};

} // namespace rim
