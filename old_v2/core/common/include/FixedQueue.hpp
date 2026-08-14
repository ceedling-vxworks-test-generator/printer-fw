#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>

namespace rim
{

//
// FixedQueue - 固定容量のスレッド安全キュー。
//
// std::queue<T> + mutex + condition_variable の置き換え。
// std::queue は push のたびにノードを動的確保するため、定常運転中にヒープを
// 触ってしまう。本クラスは固定長リングなので **確保は初期化時の1回だけ**
// (メンバとして持てば確保ゼロ)。
//
// 満杯時の方針:
//   最も古い要素を捨てて新しい要素を入れる。RIM が流すのは「最新の状態」であり、
//   詰まったときに古い状態を残しても意味がないため。捨てたことは DroppedCount()
//   で観測できる(消費が追いついていないことの検知に使う)。
//
// 制約: 要素型 T は既定構築可能・コピー代入可能であること。
//
template <typename T, std::size_t Capacity>
class FixedQueue
{
public:

    void Push(const T& value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (count_ == Capacity) {
                head_ = Next(head_);
                --count_;
                ++dropped_;
            }

            slots_[tail_] = value;
            tail_         = Next(tail_);
            ++count_;
        }

        cv_.notify_one();
    }

    bool TryPop(T& out)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (count_ == 0) return false;

        out   = slots_[head_];
        head_ = Next(head_);
        --count_;

        return true;
    }

    // 要素が入るか Shutdown されるまで待つ。
    // Shutdown 済みで空なら false(ワーカの終了条件になる)。
    bool WaitAndPop(T& out)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(
            lock,
            [this] { return shutdown_ || count_ != 0; });

        if (shutdown_ && count_ == 0) return false;

        out   = slots_[head_];
        head_ = Next(head_);
        --count_;

        return true;
    }

    // 要素が入るか、timeout が経過するか、Shutdown されるまで待つ。
    // 戻り値 false は「timeout」と「Shutdown 済みで空」のどちらも意味する
    // (呼出側は running フラグ等、別の手段で終了を判定すること)。
    // 単一のキューを待つだけでは終了検知できない複数キュー監視のために使う。
    template <typename Rep, typename Period>
    bool WaitAndPopFor(
        const std::chrono::duration<Rep, Period>& timeout,
        T& out)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (!cv_.wait_for(
                lock,
                timeout,
                [this] { return shutdown_ || count_ != 0; }))
        {
            return false;
        }

        if (shutdown_ && count_ == 0) return false;

        out   = slots_[head_];
        head_ = Next(head_);
        --count_;

        return true;
    }

    void Shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shutdown_ = true;
        }

        cv_.notify_all();
    }

    std::size_t Size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

    bool Empty() const { return Size() == 0; }

    // 溢れて捨てた件数の累計。0 でなければ消費が追いついていない。
    std::size_t DroppedCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return dropped_;
    }

    static constexpr std::size_t CapacityValue() { return Capacity; }

private:

    static std::size_t Next(std::size_t index) { return (index + 1) % Capacity; }

    mutable std::mutex      mutex_;
    std::condition_variable cv_;

    T           slots_[Capacity]{};
    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t count_{0};
    std::size_t dropped_{0};
    bool        shutdown_{false};
};

} // namespace rim
