#pragma once

#include "DomainId.hpp"
#include "DataDomainMap.hpp"
#include <vector>

#include "ProductDefinition.hpp"
#include "CapabilityItemDefinition.hpp"

namespace rim
{

class CapabilityDomainResolver
{
public:

    explicit CapabilityDomainResolver(
        const ProductDefinition& product);

    std::vector<DomainId>
    ResolveDomains(
        const CapabilityItemDefinition&
            capability) const;

private:

    const ProductDefinition&
        product_;

    DataDomainMap
        domainMap_;

};

}