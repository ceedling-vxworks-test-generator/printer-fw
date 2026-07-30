#pragma once

//
// SourceUnit - 受理した生値が「どの単位/表現で送られてきたか」を示す汎用の識別子。
//
// RIM は「同じ id でも入力元によって単位が異なる」ケースを、id を増やさず context で
// 吸収する。Rule は SourceUnit を見て分岐し、内部統一表現(正規化済みの値)へ変換する。
//   例) 温度: 華氏で来ても摂氏で来ても、DataStore へはケルビンで格納する
//
// kNormalized(既定) は「既に正規化済み。変換不要」を意味する。context に単位が
// 指定されなかった場合もこの扱いになる(単位不明を推測しない = 勝手な換算をしない)。
//
// 温度専用ではなく汎用の枠として定義する。将来 圧力(Pa/psi)・長さ(mm/inch) 等を
// 追加する場合もここへ列挙子を足す(Rule は id で選ばれるため、その id に無関係な
// 単位が来た場合は Rule 側で変換不能として弾く)。
//

namespace rim
{

enum class SourceUnit
{
    kNormalized = 0,  // 既に正規化済み(変換しない)。context 未指定時の既定。
    kCelsius,         // 摂氏 [°C]
    kFahrenheit       // 華氏 [°F]
};

} // namespace rim
