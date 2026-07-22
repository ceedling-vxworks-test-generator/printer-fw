#pragma once

//
// PrinterA の Rule 群(機種可変の正規化規則)。IRule 実装。状態を持たない。
// スカラ入力(RawValue::kScalar)を正規化 RIMValue へ変換する。
//   CelsiusRule  : 生値 → 摂氏×100(scale適用)
//   PercentRule  : 生値 → 0..100 クランプ
//   ProgressRule : 生値 → JobProgress(0..100)
//   FaultCodeRule: 生値 → FaultCode
//   BoolRule     : 生値!=0 → Bool
//   CountRule    : 生値 → UInt32
// 構造体/配列を扱う Rule の例は「新規ルール追加」ドキュメント / adapter単体テストを参照。
//

#include <cstdint>

#include "adapter/IRule.hpp"

namespace rim
{

class CelsiusRule final : public IRule
{
public:
    std::optional<RIMValue> Convert(const RawValue& raw, const DataContext& ctx) const override
    {
        if (raw.kind != RawValue::Kind::kScalar) return std::nullopt;
        const std::int32_t scale = ctx.scaleX1000 ? *ctx.scaleX1000 : 1000;
        return RIMValue::CelsiusX100(static_cast<std::int32_t>((raw.scalar * scale / 1000.0) * 100.0));
    }
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
