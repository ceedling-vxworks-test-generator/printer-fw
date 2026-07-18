#pragma once

//
// FixedVector<T,N> - 固定容量・静的確保の可変長コンテナ。
// rim/rim_fixed_vector.h からの移植(要件4-a)。std::vector 相当だが再確保・動的確保なし。
//

#include <cstddef>

namespace rim
{

template <typename T, std::size_t N>
class FixedVector
{
public:

    FixedVector()
        : size_(0)
    {
    }

    bool PushBack(const T& v)
    {
        if (size_ >= N) return false;
        data_[size_] = v;
        ++size_;
        return true;
    }

    std::size_t Size()     const { return size_; }
    std::size_t Capacity() const { return N; }
    const T&    At(std::size_t i) const { return data_[i]; }
    void        Clear() { size_ = 0; }

private:

    T           data_[N];
    std::size_t size_;
};

} // namespace rim
