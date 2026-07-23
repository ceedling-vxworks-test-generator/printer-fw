#pragma once

//
// Queue<T,N> - スレッドセーフFIFO(RingBuffer + std::mutex)。
// 仕様の FaultInputQueue / OperationReportQueue の実体(IQueue<DataEntryItem>相当)。
//   - Enqueue : 満杯なら false(喪失)。呼び出し側は喪失検知に用いる。
//   - Dequeue : 空なら false。
//

#include <cstddef>
#include <mutex>

#include "container/RingBuffer.hpp"

namespace rim
{

template <typename T, std::size_t N>
class Queue
{
public:

    bool Enqueue(const T& v)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        return ring_.Push(v);
    }

    bool Dequeue(T& out)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        return ring_.Pop(out);
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lk(mutex_);
        return ring_.Empty();
    }

    std::size_t Size() const
    {
        std::lock_guard<std::mutex> lk(mutex_);
        return ring_.Size();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lk(mutex_);
        ring_.Clear();
    }

private:

    mutable std::mutex        mutex_;
    RingBuffer<T, N>          ring_;
};

} // namespace rim
