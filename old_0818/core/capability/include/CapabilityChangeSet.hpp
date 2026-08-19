#pragma once

#include <vector>

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