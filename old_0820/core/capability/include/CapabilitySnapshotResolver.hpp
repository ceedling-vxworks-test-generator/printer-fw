#pragma once

#include <vector>

#include "RIId.hpp"
#include "DomainId.hpp"

#include "CapabilityDependencyMap.hpp"
#include "CapabilityDomainMap.hpp"

namespace rim
{

class CapabilitySnapshotResolver
{
public:

    CapabilitySnapshotResolver(
        const ProductDefinition& product)
        :
        dependencyMap_(product),
        domainMap_(product)
    {
    }

    std::vector<DomainId>
    ResolveSnapshotDomains(
        RIDataId changedDataId) const;

private:

    CapabilityDependencyMap
        dependencyMap_;

    CapabilityDomainMap
        domainMap_;
};

} // namespace rim