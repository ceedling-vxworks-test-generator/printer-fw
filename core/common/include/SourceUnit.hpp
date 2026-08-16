#pragma once

//
// SourceUnit - 生値が「どの単位/表現で送られてきたか」を示す汎用の識別子。
//
// 同じ id へ摂氏でも華氏でも投入できるようにするための context。
// DataItemDefinition::normalize は第2引数(context)を
// `static_cast<const SourceUnit*>(context)` として解釈し、変換方法を分岐する
// (context を必要としない normalize 関数は単に無視してよい)。
//
// kNormalized(既定)は「既に正規化済み。換算しない」を意味する。context 省略時
// (nullptr)もこの扱いになる(単位不明を推測して勝手に換算はしない)。
//

namespace rim
{

enum class SourceUnit
{
    kNormalized = 0,
    kCelsius,
    kFahrenheit
};

} // namespace rim
