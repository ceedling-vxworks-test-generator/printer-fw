#pragma once

#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"

namespace rim::test
{

inline void RegisterAllDomains(
    PartitionStorageRegistry& store,
    const ProductContext& context)
{
    store.RegisterDomains(
        context.GetCapabilityDomainIds());

    store.RegisterDomains(
        context.GetFacadeDomainIds());

    const auto domains =
        context.GetDomains();

    for (DomainId id = 1;
        id <= static_cast<DomainId>(
                domains.size());
        ++id)
    {
        store.RegisterDomain(
            id);
    }
}

}