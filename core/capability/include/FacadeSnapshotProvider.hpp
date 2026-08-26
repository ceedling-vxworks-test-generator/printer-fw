#pragma once

#include <cstddef>

#include "IFacadeSnapshotProvider.hpp"

#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"
#include "FacadeItemDefinition.hpp"
#include "RIMSnapshot.hpp"
#include "ProductContext.hpp"

namespace rim
{

class FacadeSnapshotProvider
    : public IFacadeSnapshotProvider
{
public:

    FacadeSnapshotProvider(
        const ProductContext& productContext,
        const PartitionStorageRegistry& store)
        :
        productContext_(productContext),
        store_(store)
    {
    }

    RIMSnapshot Create(
        RIFacadeId facadeId) const override
    {
        //
        // TEMPORARY IMPLEMENTATION
        //
        // Facade は Capability を入力として構築されるが、
        // CapabilityDomain が未実装のため、現在は
        // CapabilityStore から直接 Snapshot を生成する。
        //
        // 将来的には以下の構成へ移行する想定:
        //
        // FacadeSnapshotProvider
        //     ↓
        // CapabilityRequiredDomainMap
        //     ↓
        // CapabilitySnapshotBuilder
        //
        // その際は Create() の実装を置き換え、
        // FindFacade() と requiredCapabilityIds の直接参照を
        // 廃止する。
        //
        RIMSnapshot snapshot;

        const auto* facade =
            productContext_.FindFacade(
                facadeId);

        if (facade == nullptr)
        {
            return snapshot;
        }

        for (std::size_t i = 0;
             i < facade->requiredCapabilityCount;
             ++i)
        {
            const auto capabilityId =
                facade->requiredCapabilityIds[i];

            const DomainId domainId =
                productContext_.FindCapabilityDomainId(
                    capabilityId);

            if (domainId ==
                kInvalidDomainId)
            {
                continue;
            }

            const auto* storage =
                store_.Find(
                    domainId);

            if (storage == nullptr)
            {
                continue;
            }

            RIMDataItem item{};

            if (!storage->Find(
                    static_cast<RIDataId>(
                        capabilityId),
                    item))
            {
                continue;
            }

            snapshot.items.push_back(
                item);
        }

        return snapshot;
    }

private:

    const ProductContext&
        productContext_;

    const PartitionStorageRegistry&
        store_;
};

} // namespace rim