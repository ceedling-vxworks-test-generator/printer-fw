#pragma once

#include <vector>

#include "RIId.hpp"
#include "DomainId.hpp"
#include "CapabilityDomainMap.hpp"

namespace rim
{

class CapabilityGroupResolver
{
public:

    explicit CapabilityGroupResolver(
        const CapabilityDomainMap& map)
        : map_(map)
    {
    }

    std::vector<DomainId>
    Resolve(
        const std::vector<RICapabilityId>&
            capabilityIds) const;

private:

    const CapabilityDomainMap&
        map_;
};

} // namespace rim