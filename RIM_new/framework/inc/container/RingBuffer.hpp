#pragma once

//
// RingBuffer<T,N> - 固定容量FIFO(喪失不可・順序保証)。head/tail/count 方式。
// 満杯時 Push は false。単体はロックを持たない(呼び出し側/Queueで排他)。
//

#include <cstddef>

namespace rim
{

template <typename T, std::size_t N>
class RingBuffer
{
public:

    RingBuffer() : head_(0), tail_(0), count_(0) {}

    bool Push(const T& v)
    {
        if (count_ >= N) return false;
        data_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        ++count_;
        return true;
    }

    bool Pop(T& out)
    {
        if (count_ == 0) return false;
        out = data_[head_];
        head_ = (head_ + 1) % N;
        --count_;
        return true;
    }

    bool        Empty() const { return count_ == 0; }
    bool        Full()  const { return count_ == N; }
    std::size_t Size()  const { return count_; }
    std::size_t Capacity() const { return N; }
    void        Clear() { head_ = 0; tail_ = 0; count_ = 0; }

private:

    T           data_[N];
    std::size_t head_;
    std::size_t tail_;
    std::size_t count_;
};

} // namespace rim
