#include "SnapshotBuilder.hpp"

#include <algorithm>

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

    std::vector<DomainId>
        sortedDomains =
            domains;

    std::sort(
        sortedDomains.begin(),
        sortedDomains.end());

    std::vector<
        const PartitionStorage*>
        storages;

    storages.reserve(
        sortedDomains.size());

    std::vector<
        std::shared_lock<
            std::shared_mutex>>
        locks;

    locks.reserve(
        sortedDomains.size());

    for (const auto domainId
         : sortedDomains)
    {
        const auto* storage =
            store_.Find(
                domainId);

        if (storage == nullptr)
        {
            continue;
        }

        storages.push_back(
            storage);

        locks.emplace_back(
            storage->Mutex());
    }

    for (const auto* storage
         : storages)
    {
        const auto items =
            storage->GetAllUnlocked();

        snapshot.items.insert(
            snapshot.items.end(),
            items.begin(),
            items.end());
    }

    return snapshot;
}

} // namespace rim