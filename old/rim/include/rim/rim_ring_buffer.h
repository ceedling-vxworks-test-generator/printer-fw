/*
 * rim_ring_buffer.h - 固定容量FIFOリングバッファ（C++・性質別キュー用）
 *
 * FAULT / OPERATION レーンのステージングに用いる「喪失不可・順序保証」のFIFO。
 *   - rim::RingBuffer<T, N> = 固定容量・静的確保のFIFO（heap不使用）。
 *   - head_/tail_/count_ 方式（要素数を独立管理し、full/empty を O(1) で判定。
 *     1スロット犠牲方式を採らないため容量Nをフルに使える）。
 *   - push/pop は O(1)。満杯時 push は false（呼び出し側が RIM_ERR_POST へ写像）。
 *   - 本ヘッダは C++ 専用。公開C ABIからは DataStore 内部でのみ利用され、C側へ漏れない。
 *
 * 「1クラス1ファイル」方針により独立させている（FixedVector とは別invariant＝FIFO）。
 */
#ifndef RIM_RING_BUFFER_H
#define RIM_RING_BUFFER_H

#ifndef __cplusplus
#error "rim_ring_buffer.h は C++ 専用です（C からは各レイヤの extern \"C\" IFを使用）"
#endif

#include <cstddef>

namespace rim {

/* 固定容量FIFO。喪失不可・順序保証のレーン（fault/operation）に用いる。 */
template <typename T, std::size_t N>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0), count_(0) {}

    /* 末尾へ投入。満杯なら false（＝投入失敗）。 */
    bool push(const T& v) {
        if (count_ >= N) return false;
        data_[tail_] = v;
        tail_ = (tail_ + 1) % N;
        ++count_;
        return true;
    }

    /* 先頭を取り出し out へ。空なら false。FIFO順。 */
    bool pop(T* out) {
        if (count_ == 0) return false;
        *out = data_[head_];
        head_ = (head_ + 1) % N;
        --count_;
        return true;
    }

    bool        empty() const { return count_ == 0; }
    bool        full()  const { return count_ == N; }
    std::size_t size() const { return count_; }
    std::size_t capacity() const { return N; }
    void        clear() { head_ = 0; tail_ = 0; count_ = 0; }

private:
    T           data_[N];
    std::size_t head_;   /* 次に pop する位置 */
    std::size_t tail_;   /* 次に push する位置 */
    std::size_t count_;  /* 現在の要素数（0..N） */
};

} /* namespace rim */

#endif /* RIM_RING_BUFFER_H */
