#include "RIMSnapshotManager.hpp"

namespace rim
{

RIMSnapshotManager::
RIMSnapshotManager(
    const DomainStorageRegistry& store)
    : store_(store)
{
}

RIMSnapshot
RIMSnapshotManager::Read() const
{
    RIMSnapshot snapshot{};

    snapshot.items =
        store_.GetAll();

    return snapshot;
}

}