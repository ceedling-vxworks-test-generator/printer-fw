#include "SnapshotStorage.hpp"

namespace rim
{

AccessId
SnapshotStorage::Store(
    const RIMSnapshot& snapshot)
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    const AccessId id =
        nextId_++;

    snapshots_[writeIndex_].id =
        id;

    snapshots_[writeIndex_].snapshot =
        snapshot;

    writeIndex_ =
        (writeIndex_ + 1)
        % kMaxSnapshots;

    if (count_ < kMaxSnapshots)
    {
        ++count_;
    }

    return id;
}

bool
SnapshotStorage::TryGet(
    AccessId id,
    RIMSnapshot& snapshot) const
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    for (std::size_t i = 0;
         i < count_;
         ++i)
    {
        if (snapshots_[i].id == id)
        {
            snapshot =
                snapshots_[i].snapshot;

            return true;
        }
    }

    return false;
}

std::size_t
SnapshotStorage::GetCount() const
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    return count_;
}

} // namespace rim