#include "FacadeRequiredCapabilityMap.hpp"

#include <algorithm>

#include "ProductDefinition.hpp"
#include "FacadeItemDefinition.hpp"
#include "CapabilityRequiredDomainMap.hpp"

namespace rim
{

FacadeRequiredCapabilityMap::FacadeRequiredCapabilityMap(
    const ProductDefinition& product)
{
    // CapabilityRequiredDomainMap capabilityRequiredDomainMap(
    //     product);

    for (std::size_t i = 0;
         i < product.facadeCount;
         ++i)
    {
        const auto& facade =
            product.facades[i];

        auto& domains =
            map_[facade.id];

        for (std::size_t j = 0;
             j < facade.requiredCapabilityCount;
             ++j)
        {
            // 暫定
            const RICapabilityId capabilityId =
                facade.requiredCapabilityIds[j];

            domains.push_back(
                capabilityId);

            // const DomainId domainId =
            //     capabilityRequiredDomainMap.Find(
            //         facade.requiredCapabilityIds[j]);

            // if (domainId ==
            //     kInvalidDomainId)
            // {
            //     continue;
            // }

            // if (std::find(
            //         domains.begin(),
            //         domains.end(),
            //         domainId)
            //     == domains.end())
            // {
            //     domains.push_back(
            //         domainId);
            // }
        }
    }
}

const std::vector<RICapabilityId>&
FacadeRequiredCapabilityMap::GetRequiredCapabilities(
    RIFacadeId facadeId) const
{
    static const std::vector<RICapabilityId>
        kEmpty;

    const auto it =
        map_.find(facadeId);

    if (it == map_.end())
    {
        return kEmpty;
    }

    return it->second;
}

} // namespace rim