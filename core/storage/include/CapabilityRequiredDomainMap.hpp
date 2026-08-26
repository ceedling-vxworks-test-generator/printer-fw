#pragma once

#include <unordered_map>
#include <vector>

#include "RIId.hpp"
#include "DomainId.hpp"

namespace rim
{

struct ProductDefinition;

class CapabilityRequiredDomainMap
{
public:

    explicit CapabilityRequiredDomainMap(
        const ProductDefinition& product);

    const std::vector<DomainId>&
    Find(
        RICapabilityId capabilityId) const;

private:

    std::unordered_map
    <
        RICapabilityId,
        std::vector<DomainId>
    >
    map_;
};

} // namespace rim