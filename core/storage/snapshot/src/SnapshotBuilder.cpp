#include "SnapshotBuilder.hpp"

#include <algorithm>
#include <iostream>

namespace rim
{

SnapshotBuilder::SnapshotBuilder(
    const PartitionStorageRegistry& store)
    :
    store_(store)
{
}

RIMSnapshot
SnapshotBuilder::Build(
    const std::vector<DomainId>& domains) const
{
    RIMSnapshot snapshot;

    for (const DomainId domainId : domains)
    {
        const auto* storage =
            store_.Find(domainId);

        if (storage == nullptr)
        {
            continue;
        }

        const auto items =
            storage->GetAll();

        snapshot.items.insert(
            snapshot.items.end(),
            items.begin(),
            items.end());
    }

    return snapshot;
}

} // namespace rim