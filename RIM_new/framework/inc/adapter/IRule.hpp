#pragma once

//
// IRule - RawValue を正規化 Value へ変換する規則(仕様 §6/§9.2)。状態を持たない。
// 受理点の型自由さは RawDataInput が double へ寄せて吸収するため、Rule は double を受ける。
//

#include <optional>

#include "datastore/RIMValue.hpp"
#include "datastore/DataContext.hpp"

namespace rim
{

class IRule
{
public:

    virtual ~IRule() = default;

    virtual std::optional<RIMValue> Convert(double raw, const DataContext& ctx) const = 0;
};

} // namespace rim
