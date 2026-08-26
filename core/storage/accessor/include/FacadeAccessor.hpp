#pragma once

#include "RIId.hpp"
#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"
#include "RIMValue.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class FacadeAccessor
{
public:

    FacadeAccessor(
        const PartitionStorageRegistry& store,
        const ProductContext& context)
        :
        store_(store),
        context_(context)
    {
    }

    bool TryGet(
        RIFacadeId facadeId,
        RIMValue& value) const
    {
        const DomainId domainId =
            context_.FindFacadeDomainId(
                facadeId);

        if (domainId ==
            kInvalidDomainId)
        {
            return false;
        }

        const auto* storage =
            store_.Find(
                domainId);

        if (storage == nullptr)
        {
            return false;
        }

        RIMDataItem item{};

        if (!storage->Find(
                static_cast<RIDataId>(
                    facadeId),
                item))
        {
            return false;
        }

        value =
            item.value;

        return true;
    }

private:

    const PartitionStorageRegistry&
        store_;

    const ProductContext&
        context_;
};

} // namespace rim