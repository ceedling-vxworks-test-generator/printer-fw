#pragma once

#include <vector>
#include "rim_capability_id.h"

namespace rim
{

struct CapabilityChangeSet
{
    std::vector<RICapabilityId>
        changedCapabilities;

    bool Empty() const
    {
        return changedCapabilities.empty();
    }
};

} // namespace rim