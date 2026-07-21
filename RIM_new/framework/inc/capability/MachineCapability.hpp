#pragma once

//
// MachineCapability - Capability通知の単位(仕様 §8.4.5 の Capability データ型)。
//   capabilities : 生成済Capability全体
//   priority     : Publish優先度
//   changedKinds : 変更のあったCapability集合
//   trigger      : 配信契機(OnChange / Event 等)
//

#include "capability/CapabilitySet.hpp"
#include "capability/CapabilityKind.hpp"
#include "capability/CapabilityPriority.hpp"
#include "publisher/PublishTrigger.hpp"

namespace rim
{

struct MachineCapability
{
    CapabilitySet      capabilities{};
    CapabilityPriority priority{CapabilityPriority::kNormal};
    CapabilityKindSet  changedKinds{0};
    PublishTrigger     trigger{PublishTrigger::kOnChange};
};

} // namespace rim
