#pragma once

#include "RIId.hpp"
#include "RIMId.hpp"
#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"
#include "RIMValue.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

class CapabilityAccessor
{
public:

    CapabilityAccessor(
        const PartitionStorageRegistry& store,
        const ProductContext& context)
        :
        store_(store),
        context_(context)
    {
    }

    bool TryGet(
    const RIMId& target,
    RIMValue& value) const;

    bool TryGet(
        RICapabilityId capabilityId,
        RIMValue& value) const
    {
        const DomainId domainId =
            context_.FindDomainId(
                {
                    RIMIdType::Capability,
                    static_cast<std::uint32_t>(
                        capabilityId)
                });
                
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
                    capabilityId),
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