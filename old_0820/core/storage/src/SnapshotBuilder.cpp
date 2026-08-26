#include "SnapshotBuilder.hpp"

#include <algorithm>
#include <iostream>

namespace rim
{

SnapshotBuilder::SnapshotBuilder(
    const DomainStorageRegistry& store,
    const ProductDefinition& product)
    :
    store_(store),
    product_(product),
    domainMap_(product)
{
}

RIMSnapshot
SnapshotBuilder::Build(
    const std::vector<DomainId>& domains) const
{
    RIMSnapshot snapshot;

    const auto items =
        store_.GetAll();

    for (const auto& item : items)
    {
        const DomainId itemDomainId =
            domainMap_.Find(
                item.id);

        if (itemDomainId ==
            kInvalidDomainId)
        {
            continue;
        }

        if (std::find(
                domains.begin(),
                domains.end(),
                itemDomainId)
            != domains.end())
        {
            snapshot.items.push_back(
                item);
        }
    }

    return snapshot;
}

} // namespace rim