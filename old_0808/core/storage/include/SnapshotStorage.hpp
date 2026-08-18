#pragma once

#include "AccessId.hpp"
#include "RIMSnapshot.hpp"

#include <array>
#include <mutex>

namespace rim
{

class SnapshotStorage
{
public:

    static constexpr std::size_t
    EntrySize()
    {
        return sizeof(Entry);
    }

    static constexpr std::size_t
    MaxSnapshots()
    {
        return kMaxSnapshots;
    }

    AccessId
    Store(
        const RIMSnapshot& snapshot);

    bool
    TryGet(
        AccessId id,
        RIMSnapshot& snapshot) const;

    std::size_t
    GetCount() const;

private:

    static constexpr std::size_t
        kMaxSnapshots = 16;

    struct Entry
    {
        AccessId id{};

        RIMSnapshot snapshot{};
    };

    mutable std::mutex mutex_;

    std::array<
        Entry,
        kMaxSnapshots>
        snapshots_{};

    AccessId nextId_{1};

    std::size_t writeIndex_{0};

    std::size_t count_{0};
};

} // namespace rim