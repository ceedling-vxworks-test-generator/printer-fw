#pragma once

#include <cstddef>

namespace rim
{

//
// FixedVector - 固定容量の可変長配列。
//
// std::vector の代替。**動的確保を一切しない**代わりに、容量を超える追加は
// 失敗する(黙って伸びない)。組込みで「定常運転中にヒープを触らない」ことを
// 保証するために使う。
//
// 制約:
//   - 要素型 T は既定構築可能・コピー代入可能であること
//     (全要素をあらかじめ構築しておくため。placement new は使わない)
//   - スレッド安全ではない。共有する場合は呼出側で排他すること。
//
template <typename T, std::size_t Capacity>
class FixedVector
{
public:

    using value_type = T;

    // 末尾に追加する。満杯なら false(要素は変化しない)。
    bool PushBack(const T& value)
    {
        if (size_ >= Capacity) return false;
        items_[size_++] = value;
        return true;
    }

    // 末尾を削る。空なら false。
    bool PopBack()
    {
        if (size_ == 0) return false;
        items_[--size_] = T{};
        return true;
    }

    // index の要素を消して詰める(順序は保たれる)。
    bool Erase(std::size_t index)
    {
        if (index >= size_) return false;

        for (std::size_t i = index; i + 1 < size_; ++i) {
            items_[i] = items_[i + 1];
        }

        items_[--size_] = T{};
        return true;
    }

    T&       operator[](std::size_t index)       { return items_[index]; }
    const T& operator[](std::size_t index) const { return items_[index]; }

    T*       begin()       { return items_; }
    T*       end()         { return items_ + size_; }
    const T* begin() const { return items_; }
    const T* end()   const { return items_ + size_; }

    std::size_t Size() const     { return size_; }
    bool        Empty() const    { return size_ == 0; }
    bool        Full() const     { return size_ >= Capacity; }
    void        Clear()          { for (std::size_t i = 0; i < size_; ++i) items_[i] = T{}; size_ = 0; }

    static constexpr std::size_t CapacityValue() { return Capacity; }

private:

    T           items_[Capacity]{};
    std::size_t size_{0};
};

} // namespace rim
