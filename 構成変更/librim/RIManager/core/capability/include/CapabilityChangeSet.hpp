#pragma once

#include <string>
#include <vector>

namespace rim
{

struct CapabilityChangeSet
{
    std::vector<std::string>
        changedCapabilities;

    bool Empty() const
    {
        return changedCapabilities.empty();
    }
};

} // namespace rim