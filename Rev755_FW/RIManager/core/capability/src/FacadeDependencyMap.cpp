#include "FacadeDependencyMap.hpp"

#include "ProductDefinition.hpp"

namespace rim
{

FacadeDependencyMap::FacadeDependencyMap(
    const ProductDefinition& product)
{
    for (std::size_t i = 0;
         i < product.facadeCount;
         ++i)
    {
        const auto& Facade =
            product.facades[i];

        for (std::size_t j = 0;
             j < Facade.requiredCapabilityCount;
             ++j)
        {
            const auto capabilityId =
                Facade.requiredCapabilityIds[j];

            map_[capabilityId].push_back(
                &Facade);
        }
    }
}

const std::vector<
    const FacadeItemDefinition*>&
FacadeDependencyMap::Find(
    RICapabilityId capabilityId) const
{
    static const std::vector<
        const FacadeItemDefinition*> kEmpty;

    const auto it =
        map_.find(capabilityId);

    if (it == map_.end())
    {
        return kEmpty;
    }

    return it->second;
}

const std::vector<
    const FacadeItemDefinition*>&
FacadeDependencyMap::Find(
    const RIMId& id) const
{
    static const std::vector<
        const FacadeItemDefinition*> kEmpty;

    if (id.type != RIMIdType::Capability)
    {
        return kEmpty;
    }

    return Find(
        static_cast<RICapabilityId>(
            id.id));
}

} // namespace rim