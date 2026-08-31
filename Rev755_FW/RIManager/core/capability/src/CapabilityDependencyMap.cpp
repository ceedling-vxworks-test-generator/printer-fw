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

            map_[{
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    dataId)
            }].push_back(
                &capability);
        }
    }
}

const std::vector<
    const CapabilityItemDefinition*>&
CapabilityDependencyMap::Find(
    const RIMId& id) const
{
    static const std::vector<
        const CapabilityItemDefinition*> kEmpty;

    if (id.type !=
        RIMIdType::Data)
    {
        return kEmpty;
    }

    const auto it =
        map_.find(id);

    if (it == map_.end())
    {
        return kEmpty;
    }

    return it->second;
}

} // namespace rim