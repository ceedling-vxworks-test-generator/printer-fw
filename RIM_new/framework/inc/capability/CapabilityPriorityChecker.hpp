#pragma once

//
// CapabilityPriorityChecker - 今回Capabilityの内容からPublish優先度を判定(仕様 §8.7)。
// TBDのため妥当な既定ポリシーを実装:
//   Critical : Error発生中 or 安全でない(Safety)
//   High     : メンテ要 or 消耗品残少
//   Normal   : Job/Env に変化
//   Low      : 上記以外
//

#include "capability/CapabilitySet.hpp"
#include "capability/CapabilityPriority.hpp"

namespace rim
{

class CapabilityPriorityChecker
{
public:

    CapabilityPriority Check(const CapabilitySet& cap) const
    {
        if ((cap.error  && cap.error->hasError) ||
            (cap.safety && !cap.safety->safe)) {
            return CapabilityPriority::kCritical;
        }
        if ((cap.maint      && cap.maint->needed) ||
            (cap.consumable && (cap.consumable->inkLow || cap.consumable->wiperLow))) {
            return CapabilityPriority::kHigh;
        }
        if (cap.job || cap.env) {
            return CapabilityPriority::kNormal;
        }
        return CapabilityPriority::kLow;
    }
};

} // namespace rim
