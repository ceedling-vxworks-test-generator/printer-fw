#pragma once

//
// SkeletonDataProfile - 新機種の id 定義テンプレート。IDataProfile 実装。
// TODO: Skeleton を機種名(例 PrinterB)へ置換し、Classify/Convert を実装する。
//

#include "adapter/IDataProfile.hpp"

namespace rim
{

class SkeletonDataProfile final
    : public IDataProfile
{
public:

    std::optional<InputKind> Classify(RIMDataId id) const override;

    std::optional<RIMValue> Convert(
        RIMDataId id,
        double raw,
        const DataContext& ctx) const override;
};

} // namespace rim
