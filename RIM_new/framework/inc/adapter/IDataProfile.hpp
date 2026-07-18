#pragma once

//
// IDataProfile - 機種可変の「id 定義」を与える抽象IF(devices が実装)。
// rim の adapter/datastore に散在していた classify()/convert() を機種側へ集約する。
//   - Classify : id → 入力性質(InputKind)。設計不備時は nullopt。
//   - Convert  : native生値(double へ集約) → id 定義に沿った正規化 RIMValue。
//                受理点の「型の自由さ」は Adapter 側で double へ寄せて吸収する。
//                ctx.scaleX1000 があれば適用する。
//

#include <cstdint>
#include <optional>

#include "datastore/RIMDataId.hpp"
#include "datastore/InputKind.hpp"
#include "datastore/RIMValue.hpp"
#include "datastore/DataContext.hpp"

namespace rim
{

class IDataProfile
{
public:

    virtual ~IDataProfile() = default;

    virtual std::optional<InputKind> Classify(RIMDataId id) const = 0;

    virtual std::optional<RIMValue> Convert(
        RIMDataId id,
        double raw,
        const DataContext& ctx) const = 0;
};

} // namespace rim
