/*
 * rim_fixed_vector.h - 固定容量・静的確保の可変長コンテナ（C++・要件4(a)）
 *
 * ここが「List可変長はC++の仕組みを用いる」箇所。
 *   - rim::FixedVector<T, N> = 固定容量・静的確保の可変長コンテナ（heap不使用）。
 *     std::vector 相当の push/size/clear を、テンプレートでボイラープレートなく提供する。
 *   - std::vector は使わない（定常時再確保・動的確保を避けるFW方針）。
 *   - 本ヘッダは C++ 専用（テンプレート）。公開C ABI（rim_list.h）からは
 *     不透明ハンドル越しに利用され、C側にC++は一切漏れない。
 *
 * 「1クラス1ファイル」方針により、唯一の実体クラスとして本ファイルに独立させている。
 */
#ifndef RIM_FIXED_VECTOR_H
#define RIM_FIXED_VECTOR_H

#ifndef __cplusplus
#error "rim_fixed_vector.h は C++ 専用です（C からは rim_list.h を使用）"
#endif

#include <cstddef>

namespace rim {

/* 固定容量の可変長コンテナ（要件4(a)のC++機構）。 */
template <typename T, std::size_t N>
class FixedVector {
public:
    FixedVector() : size_(0) {}

    bool push_back(const T& v) {
        if (size_ >= N) return false;
        data_[size_] = v;
        ++size_;
        return true;
    }
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return N; }
    const T& at(std::size_t i) const { return data_[i]; }
    void clear() { size_ = 0; }

private:
    T           data_[N];
    std::size_t size_;
};

} /* namespace rim */

#endif /* RIM_FIXED_VECTOR_H */
