#include "CapabilityDependencyMap.hpp"

#include "ProductDefinition.hpp"

namespace rim
{

CapabilityDependencyMap::CapabilityDependencyMap(
    const ProductDefinition& product)
{
    for (std::size_t i = 0;
         i < product.capabilityCount;
         ++i)
    {
        const auto& capability =
            product.capabilities[i];

        for (std::size_t j = 0;
             j < capability.requiredDataCount;
             ++j)
        {
            const auto dataId =
                capability.requiredDataIds[j];

            map_[dataId].push_back(
                &capability);
        }
    }
}

const std::vector<
    const CapabilityItemDefinition*>&
CapabilityDependencyMap::Find(
    RIDataId dataId) const
{
    static const std::vector<
        const CapabilityItemDefinition*> kEmpty;

    const auto it =
        map_.find(dataId);

    if (it == map_.end())
    {
        return kEmpty;
    }

    return it->second;
}

} // namespace rim