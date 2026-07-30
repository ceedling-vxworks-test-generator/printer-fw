#pragma once

//
// PrinterA の Rule 群(機種可変の正規化規則)。IRule 実装。状態を持たない。
// スカラ入力(RawValue::kScalar)を正規化 RIMValue へ変換する。
//   TemperatureRule : 生値 → 絶対温度(ケルビン)×100。context.unit で入力単位を分岐(scale適用)
//   PercentRule  : 生値 → 0..100 クランプ
//   ProgressRule : 生値 → JobProgress(0..100)
//   FaultCodeRule: 生値 → FaultCode
//   BoolRule     : 生値!=0 → Bool
//   CountRule    : 生値 → UInt32
// 構造体/配列を扱う Rule の例は「新規ルール追加」ドキュメント / adapter単体テストを参照。
//

#include <cstdint>
#include <cmath>

#include "adapter/IRule.hpp"

#include "datastore/SourceUnit.hpp"

namespace rim
{

//
// TemperatureRule - 温度を絶対温度(ケルビン)×100 へ正規化する。
//
// 同じ id へ摂氏でも華氏でも投入でき、どちらで来たかは context.unit で判別する
// (id を単位ごとに増やさずに済ませるための分岐)。
//   kCelsius    : K = C + 273.15
//   kFahrenheit : K = (F - 32) × 5/9 + 273.15
//   kNormalized : 既にケルビンとみなしてそのまま採用(context 未指定時もこれ)
//   上記以外    : この id にとって無関係な単位 → 変換不能(nullopt)
//
// 丸めについて: 華氏→摂氏の (F-32)×5/9 は割り切れないため、切り捨てると常に下振れする。
// 本 Rule のみ std::lround による四捨五入を採る(他の Rule は 0方向への切り捨て)。
// 例) 100°F = 310.9277…K → 31093(切り捨てなら 31092)。
//
class TemperatureRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext& ctx) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;

        const std::int32_t scale  = ctx.scaleX1000 ? *ctx.scaleX1000 : 1000;
        const double       scaled = raw.scalar * scale / 1000.0;
        const SourceUnit   unit   = ctx.unit ? *ctx.unit : SourceUnit::kNormalized;

        double kelvin = 0.0;
        switch (unit) {
            case SourceUnit::kCelsius:    kelvin = scaled + kAbsoluteZeroOffsetC; break;
            case SourceUnit::kFahrenheit: kelvin = (scaled - 32.0) * 5.0 / 9.0 + kAbsoluteZeroOffsetC; break;
            case SourceUnit::kNormalized: kelvin = scaled; break;   // 既に正規化済み
            default:                      return std::nullopt;      // 温度に無関係な単位
        }

        return RIMValue::KelvinX100(static_cast<std::int32_t>(std::lround(kelvin * 100.0)));
    }

private:
    static constexpr double kAbsoluteZeroOffsetC = 273.15;  // 0°C の絶対温度[K]
};

class PercentRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        long p = static_cast<long>(raw.scalar);
        if (p < 0) p = 0; if (p > 100) p = 100;
        return RIMValue::Percent(static_cast<std::uint8_t>(p));
    }
};

class ProgressRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        long p = static_cast<long>(raw.scalar);
        if (p < 0) p = 0; if (p > 100) p = 100;
        return RIMValue::JobProgress(static_cast<std::uint8_t>(p));
    }
};

class FaultCodeRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::FaultCode(static_cast<std::uint32_t>(raw.scalar));
    }
};

class BoolRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::Bool(raw.scalar != 0.0);
    }
};

class CountRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        return RIMValue::UInt32(static_cast<std::uint32_t>(raw.scalar));
    }
};

} // namespace rim
