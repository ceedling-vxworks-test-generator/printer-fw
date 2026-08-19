#include "CapabilityGroupResolver.hpp"

#include <algorithm>

namespace rim
{

std::vector<DomainId>
CapabilityGroupResolver::Resolve(
    const std::vector<RICapabilityId>&
        capabilityIds) const
{
    std::vector<DomainId> result;

    for (const auto capabilityId
         : capabilityIds)
    {
        const auto& groups =
            map_.Find(
                capabilityId);

        for (const auto group
             : groups)
        {
            if (std::find(
                    result.begin(),
                    result.end(),
                    group)
                == result.end())
            {
                result.push_back(
                    group);
            }
        }
    }

    return result;
}

} // namespace rim