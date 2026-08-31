#pragma once

#include <cstddef>

#include "IFacadeSnapshotProvider.hpp"

#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"
#include "FacadeItemDefinition.hpp"
#include "RIMSnapshot.hpp"
#include "ProductContext.hpp"
#include "SnapshotAccessor.hpp"

namespace rim
{

class FacadeSnapshotProvider
    : public IFacadeSnapshotProvider
{
public:

    FacadeSnapshotProvider(
        const ProductContext& productContext,
        const SnapshotAccessor& accessor)
        :
        productContext_(productContext),
        accessor_(accessor)
    {
    }

    RIMSnapshot Create(
        RIFacadeId facadeId) const override
    {
        const auto* facade =
            productContext_.FindFacade(
                {
                    RIMIdType::Facade,
                    static_cast<std::uint32_t>(
                        facadeId)
                });
                
        if (facade == nullptr)
        {
            return {};
        }

        std::vector<DomainId>
            domains;

        domains.reserve(
            facade->requiredCapabilityCount);

        for (std::size_t i = 0;
            i < facade->requiredCapabilityCount;
            ++i)
        {
            const auto capabilityId =
                facade->requiredCapabilityIds[i];

            const DomainId domainId =
                productContext_.FindDomainId(
                    {
                        RIMIdType::Capability,
                        static_cast<std::uint32_t>(
                            capabilityId)
                    });
                    
            if (domainId ==
                kInvalidDomainId)
            {
                continue;
            }

            domains.push_back(
                domainId);
        }

        return accessor_.CreateSnapshot(
            domains);
    }

private:

    const ProductContext&
        productContext_;

    const SnapshotAccessor&
        accessor_;
};

} // namespace rim