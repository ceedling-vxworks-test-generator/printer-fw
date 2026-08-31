#pragma once

#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

namespace rim::test
{

inline RIMDataItem CreateDoubleItem(
    RIDataId id,
    double value)
{
    RIMDataItem item{};

    item.id =
        id;

    item.value =
        RIMValueFactory::CreateDouble(
            value);

    return item;
}

inline RIMDataItem CreateInt32Item(
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id =
        id;

    item.value =
        RIMValueFactory::CreateInt32(
            value);

    return item;
}

inline RIMDataItem CreateBoolItem(
    RIDataId id,
    bool value)
{
    RIMDataItem item{};

    item.id =
        id;

    item.value =
        RIMValueFactory::CreateBool(
            value);

    return item;
}

inline void StoreDouble(
    PartitionStorage& storage,
    RIDataId id,
    double value)
{
    storage.Store(
        CreateDoubleItem(
            id,
            value));
}

inline void StoreBool(
    PartitionStorage& storage,
    RIDataId id,
    bool value)
{
    storage.Store(
        CreateBoolItem(
            id,
            value));
}

} // namespace rim::test