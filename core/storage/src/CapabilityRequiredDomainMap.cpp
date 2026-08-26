#include "CapabilityRequiredDomainMap.hpp"

#include <algorithm>

#include "ProductDefinition.hpp"
#include "CapabilityItemDefinition.hpp"
#include "DataDomainMap.hpp"

namespace rim
{

CapabilityRequiredDomainMap::CapabilityRequiredDomainMap(
    const ProductDefinition& product)
{
    DataDomainMap dataDomainMap(
        product);

    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        const auto& capability =
            product.capabilities[i];

        auto& domains =
            map_[capability.id];

        for (std::size_t j = 0;
             j < capability.requiredDataCount;
             ++j)
        {
            const DomainId domainId =
                dataDomainMap.Find(
                    capability.requiredDataIds[j]);

            if (domainId ==
                kInvalidDomainId)
            {
                continue;
            }

            if (std::find(
                    domains.begin(),
                    domains.end(),
                    domainId)
                == domains.end())
            {
                domains.push_back(
                    domainId);
            }
        }
    }
}

const std::vector<DomainId>&
CapabilityRequiredDomainMap::Find(
    RICapabilityId capabilityId) const
{
    static const std::vector<DomainId>
        kEmpty;

    const auto it =
        map_.find(capabilityId);

    if (it == map_.end())
    {
        return kEmpty;
    }

    return it->second;
}

} // namespace rim