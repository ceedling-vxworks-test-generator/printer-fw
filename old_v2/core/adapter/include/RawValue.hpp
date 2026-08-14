#pragma once

//
// RawValue - RIM_AdapterLayer 受理点の「型自由な生値」(仕様 §7.1 / 実装方針4-b)。
// スカラ(数値)に加え、構造体・配列を非所有のバイト列参照として受け取れる。
//   - kScalar : i32/u32/double を double へ集約して保持
//   - kBytes  : 構造体/配列の先頭ポインタ＋サイズ＋要素数を参照(コピーしない)
// RawValue は Push 呼び出し中のみ有効(呼び出し側が実体を所有)。Rule は同期的に
// 正規化 RIMValue へ変換するため参照期間内で完結する。
//

#include <cstddef>

namespace rim
{

struct RawValue
{
    enum class Kind { kScalar, kBytes };

    Kind        kind{Kind::kScalar};
    double      scalar{0.0};      // kScalar
    const void* data{nullptr};    // kBytes: 構造体/配列の先頭(非所有)
    std::size_t size{0};          // kBytes: 総バイト長
    std::size_t count{0};         // kBytes: 要素数(配列=n、構造体=1)

    static RawValue Scalar(double v)
    {
        RawValue r; r.kind = Kind::kScalar; r.scalar = v; return r;
    }

    template <typename T>
    static RawValue Struct(const T& s)
    {
        RawValue r; r.kind = Kind::kBytes; r.data = &s; r.size = sizeof(T); r.count = 1; return r;
    }

    template <typename T>
    static RawValue Array(const T* a, std::size_t n)
    {
        RawValue r; r.kind = Kind::kBytes; r.data = a; r.size = sizeof(T) * n; r.count = n; return r;
    }

    // kBytes を型 T として解釈するヘルパ。
    template <typename T>
    const T* As() const { return static_cast<const T*>(data); }

    template <typename T>
    const T& ElementAt(std::size_t i) const { return As<T>()[i]; }
};

} // namespace rim
