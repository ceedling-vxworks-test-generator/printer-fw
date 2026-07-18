#pragma once

#include "adapter/rule/ISensorRule.hpp"

namespace rim
{

//
// TODO: Skeleton を機種/センサ名に置換。
// センサ生値 → 正規化値 への変換ルール。
//
class SkeletonSensorRule final
    : public ISensorRule
{
public:

    RIMValue Execute(
        const RIMValue& input) const override
    {
        // TODO: 実変換を実装する(例: 単位換算)。
        return input;
    }
};

} // namespace rim
