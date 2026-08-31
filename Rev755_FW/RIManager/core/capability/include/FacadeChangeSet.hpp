#pragma once

#include <vector>

namespace rim
{

struct FacadeChangeSet
{
    std::vector<RIFacadeId>
        changedFacades;

    bool Empty() const
    {
        return changedFacades.empty();
    }
};

} // namespace rim