#pragma once

//
// FixedMap<K,V,N> - 固定容量キー付きコレクション(管理配列)。FaultRegistry の実体。
// キーはユニーク。Add:重複/満杯でfalse、Update/Remove:不在でfalse。Remove は swap-remove。
//

#include <cstddef>

namespace rim
{

template <typename K, typename V, std::size_t N>
class FixedMap
{
public:

    FixedMap() : count_(0) {}

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

    bool Add(const K& key, const V& val)
    {
        if (Contains(key)) return false;
        if (count_ >= N) return false;
        keys_[count_] = key;
        vals_[count_] = val;
        ++count_;
        return true;
    }

    bool Update(const K& key, const V& val)
    {
        V* p = Find(key);
        if (!p) return false;
        *p = val;
        return true;
    }

    bool Upsert(const K& key, const V& val)
    {
        V* p = Find(key);
        if (p) { *p = val; return true; }
        return Add(key, val);
    }

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
