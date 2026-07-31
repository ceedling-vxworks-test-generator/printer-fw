#include "AggregateRIMSnapshotReader.hpp"

namespace rim
{

AggregateRIMSnapshotReader::
AggregateRIMSnapshotReader(
    const ValueStore& store)
    : store_(store)
{
}

RIMSnapshot
AggregateRIMSnapshotReader::Read() const
{
    RIMSnapshot snapshot{};

    // 旧実装は store_.GetAll() の返り値(std::vector)を代入していたため、
    // Snapshot を作るたびに動的確保が起きていた。
    store_.CopyAllTo(
        snapshot.items);

    return snapshot;
}

}
