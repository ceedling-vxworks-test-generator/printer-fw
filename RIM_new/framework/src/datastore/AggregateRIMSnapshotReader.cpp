#include "datastore/AggregateRIMSnapshotReader.hpp"

#include "datastore/RIMValueAccessor.hpp"

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

    RIMDataItem item{};

    if (store_.Find(
            RIMDataId::kTemperature,
            item))
    {
        RIMValueAccessor::GetDouble(
            item.value,
            snapshot.temperature);
    }

    if (store_.Find(
            RIMDataId::kHumidify,
            item))
    {
        RIMValueAccessor::GetDouble(
            item.value,
            snapshot.humidity);
    }

    if (store_.Find(
            RIMDataId::kDoorUpper,
            item))
    {
        RIMValueAccessor::GetBool(
            item.value,
            snapshot.upperDoorOpen);
    }

    if (store_.Find(
            RIMDataId::kDoorRight,
            item))
    {
        RIMValueAccessor::GetBool(
            item.value,
            snapshot.rightDoorOpen);
    }

    if (store_.Find(
            RIMDataId::kDoorLeft,
            item))
    {
        RIMValueAccessor::GetBool(
            item.value,
            snapshot.leftDoorOpen);
    }

    return snapshot;
}

}