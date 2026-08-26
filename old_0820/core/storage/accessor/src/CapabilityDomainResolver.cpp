#include <algorithm>

#include "CapabilityDomainResolver.hpp"

namespace rim
{
CapabilityDomainResolver::
CapabilityDomainResolver(
    const ProductDefinition& product)
    :
    product_(product),
    domainMap_(product)
{
}

std::vector<DomainId>
CapabilityDomainResolver::
ResolveDomains(
    const CapabilityItemDefinition&
        capability) const
{
    std::vector<DomainId>
        result;

    for (std::size_t i = 0;
         i < capability.requiredDataCount;
         ++i)
    {
        const DomainId domainId =
            domainMap_.Find(
                capability.requiredDataIds[i]);

        if (domainId ==
            kInvalidDomainId)
        {
            continue;
        }

        if (std::find(
                result.begin(),
                result.end(),
                domainId)
            == result.end())
        {
            result.push_back(
                domainId);
        }
    }

    return result;
}

}