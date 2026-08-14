#pragma once

//
// PrinterAContextKeys - PrinterA機種のnormalize関数がRIMContextから読む
// キー・値の取り決め。RIMContextはkey文字列+int値という汎用形式のため、
// 機種側でこの対応を1箇所にまとめておく(表記揺れ防止)。
//

namespace rim
{

constexpr const char* kContextKeyUnit = "unit";

// kContextKeyUnit の値。
enum PrinterAUnit
{
    kUnitNormalized = 0, // 既に正規化済み(ケルビン)として扱う。未指定時のデフォルトもこれ。
    kUnitCelsius    = 1,
    kUnitFahrenheit = 2,
};

} // namespace rim
