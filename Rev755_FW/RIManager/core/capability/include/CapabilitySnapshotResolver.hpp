#pragma once

#include <vector>

#include "RIId.hpp"
#include "DomainId.hpp"

#include "CapabilityDependencyMap.hpp"
#include "CapabilityRequiredDomainMap.hpp"
#include "ProductContext.hpp"

namespace rim
{

class CapabilitySnapshotResolver
{
public:

    explicit CapabilitySnapshotResolver(
        const ProductContext& context);

    std::vector<DomainId>
    ResolveSnapshotDomains(
        RICapabilityId capabilityId) const;

private:

    const CapabilityDependencyMap&
        dependencyMap_;

    const CapabilityRequiredDomainMap&
        domainMap_;
    };

} // namespace rim