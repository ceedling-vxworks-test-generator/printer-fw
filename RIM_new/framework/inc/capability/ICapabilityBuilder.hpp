#pragma once

//
// ICapabilityBuilder - SnapshotからCapabilityを生成する(仕様 §8.5)。
// 判定式(閾値等)は機種可変のため devices が実装する。
//

#include "capability/CapabilitySet.hpp"
#include "datastore/Snapshot.hpp"

namespace rim
{

class ICapabilityBuilder
{
public:

    virtual ~ICapabilityBuilder() = default;

    virtual CapabilitySet Build(const MachineSnapshot& snapshot) const = 0;
};

} // namespace rim
