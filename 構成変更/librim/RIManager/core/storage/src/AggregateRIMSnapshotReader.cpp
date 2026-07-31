#include "AggregateRIMSnapshotReader.hpp"

#include "RIMValueAccessor.hpp"

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

    snapshot.items =
        store_.GetAll();

    return snapshot;
}

}