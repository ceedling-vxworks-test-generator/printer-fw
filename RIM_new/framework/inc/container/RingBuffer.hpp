#pragma once

//
// RingBuffer<T,N> - 固定容量FIFO(喪失不可・順序保証のレーン用)。
// rim/rim_ring_buffer.h からの移植。head/tail/count 方式で容量Nをフル使用。
// push/pop は O(1)。満杯時 push は false(呼び出し側が RIM_ERR_POST へ写像)。
//

#include <cstddef>

namespace rim
{

template <typename T, std::size_t N>
class RingBuffer
{
public:

    RingBuffer()
        : head_(0)
        , tail_(0)
        , count_(0)
    {
    }

    // 末尾へ投入。満杯なら false。
    bool Push(const T& v)
    {
        if (count_ >= N) return false;
        data_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        ++count_;
        return true;
    }

    // 先頭を取り出し out へ。空なら false。FIFO順。
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
    std::size_t head_;   // 次に Pop する位置
    std::size_t tail_;   // 次に Push する位置
    std::size_t count_;  // 現在の要素数(0..N)
};

} // namespace rim
