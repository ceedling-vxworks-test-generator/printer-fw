#include "RIMSnapshotManager.hpp"

#include "RIMValueAccessor.hpp"

namespace rim
{

RIMSnapshotManager::
RIMSnapshotManager(
    const ValueStore& store)
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