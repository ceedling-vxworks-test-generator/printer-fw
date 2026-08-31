#include "CapabilitySnapshotResolver.hpp"
#include "ProductContext.hpp"

#include <algorithm>

namespace rim
{

    CapabilitySnapshotResolver::
    CapabilitySnapshotResolver(
        const ProductContext& context)
        :
        dependencyMap_(
            context.AffectedCapabilities()),
        domainMap_(
            context.CapabilityRequiredDomains())
    {
    }

std::vector<DomainId> CapabilitySnapshotResolver::ResolveSnapshotDomains(RICapabilityId capabilityId) const{
    return domainMap_.Find(capabilityId);
}

} // namespace rim