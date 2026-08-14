#pragma once

//
// PrinterA の Rule 群(機種可変の正規化規則)。IRule 実装。**状態を持たない**。
//
// Adapter が受けた生値(RawValue)を、後段が扱う内部統一表現(RIMValue)へ
// 正規化する。後段は正規化済みの値しか見ないため、単位や生値の癖はここで吸収する。
//
//   TemperatureRule : 生値 -> 絶対温度(ケルビン)。context.unit で入力単位を分岐
//   PercentRule     : 生値 -> 0..100 にクランプ
//   ProgressRule    : 生値 -> 進捗 0..100 にクランプ
//   FaultCodeRule   : 生値 -> 異常コード(UInt32)
//   BoolRule        : 生値 != 0 -> Bool
//   CountRule       : 生値 -> UInt32
//

#include <cmath>
#include <cstdint>

#include "IRule.hpp"
#include "RIMValueFactory.hpp"
#include "SourceUnit.hpp"

namespace rim
{

//
// TemperatureRule - 温度を絶対温度(ケルビン)へ正規化する。
//
// 同じ id へ摂氏でも華氏でも投入でき、どちらで来たかは context.unit で判別する
// (id を単位ごとに増やさずに済ませるための分岐)。
//   kCelsius    : K = C + 273.15
//   kFahrenheit : K = (F - 32) * 5/9 + 273.15
//   kNormalized : 既にケルビンとみなしてそのまま採用(context 未指定時もこれ)
//   上記以外    : この id にとって無関係な単位 -> 変換不能(nullopt)
//
// 丸めについて: 華氏->摂氏の (F-32)*5/9 は割り切れない。double のまま保持するので
// ここでは丸めないが、整数へ落とす利用側は切り捨てではなく四捨五入を使うこと
// (切り捨ては常に下振れする)。
//
class TemperatureRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext& ctx) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        const std::int32_t scale =
            ctx.scaleX1000 ? *ctx.scaleX1000 : 1000;

        const double scaled =
            raw.scalar * scale / 1000.0;

        const SourceUnit unit =
            ctx.unit ? *ctx.unit : SourceUnit::kNormalized;

        double kelvin = 0.0;

        switch (unit)
        {
        case SourceUnit::kCelsius:
            kelvin = scaled + kAbsoluteZeroOffsetC;
            break;

        case SourceUnit::kFahrenheit:
            kelvin = (scaled - 32.0) * 5.0 / 9.0 + kAbsoluteZeroOffsetC;
            break;

        case SourceUnit::kNormalized:
            kelvin = scaled;   // 既に正規化済み
            break;

        default:
            return std::nullopt;   // 温度に無関係な単位
        }

        return RIMValueFactory::CreateDouble(
            kelvin);
    }

private:

    static constexpr double kAbsoluteZeroOffsetC = 273.15;  // 0°C の絶対温度[K]
};

// 0..100 にクランプする共通処理。
namespace detail
{

inline std::int32_t ClampPercent(
    double value)
{
    if (value < 0.0)
    {
        return 0;
    }

    if (value > 100.0)
    {
        return 100;
    }

    return static_cast<std::int32_t>(value);
}

} // namespace detail

class PercentRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateInt32(
            detail::ClampPercent(
                raw.scalar));
    }
};

class ProgressRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateInt32(
            detail::ClampPercent(
                raw.scalar));
    }
};

class FaultCodeRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateUInt32(
            static_cast<std::uint32_t>(
                raw.scalar));
    }
};

class BoolRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateBool(
            raw.scalar != 0.0);
    }
};

class CountRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateInt32(
            static_cast<std::int32_t>(
                raw.scalar));
    }
};

} // namespace rim
