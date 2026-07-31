#pragma once

//
// IRule - RawValue を正規化 Value へ変換する規則(仕様 §6/§9.2)。状態を持たない。
// 受理点の型自由さ(スカラ/構造体/配列)は RawValue が吸収する。
// Rule は外部形式を内部統一表現 RIMValue へ正規化する(後続層は RIMValue のみ扱う)。
//

#include <optional>

#include "RawValue.hpp"

#include "RIMValue.hpp"
#include "DataContext.hpp"

namespace rim
{

class IRule
{
public:

    virtual ~IRule() = default;

    virtual std::optional<RIMValue> Convert(const RawValue& raw, const DataContext& ctx) const = 0;
};

} // namespace rim
