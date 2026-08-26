#pragma once

#include <cstdint>
#include <stdexcept>

#include "printer_a.h"
#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

inline void AddDoubleItem(
    RIMSnapshot& snapshot,
    RIDataId id,
    double value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kDouble;

    item.value =
        RIMValueFactory::CreateDouble(
            value);

    snapshot.items.push_back(item);
}

inline void AddBoolItem(
    RIMSnapshot& snapshot,
    RIDataId id,
    bool value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kBool;

    item.value =
        RIMValueFactory::CreateBool(
            value);

    snapshot.items.push_back(item);
}

inline void AddInt32Item(
    RIMSnapshot& snapshot,
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kInt32;

    item.value =
        RIMValueFactory::CreateInt32(
            value);

    snapshot.items.push_back(item);
}

inline double GetDouble(
    const RIMSnapshot& snapshot,
    RIDataId id)
{
    double value{};

    if (!snapshot.TryGetDouble(
            id,
            value))
    {
        ADD_FAILURE()
            << "double not found";

        return {};
    }

    return value;
}

inline bool GetBool(
    const RIMSnapshot& snapshot,
    RIDataId id)
{
    bool value{};

    if (!snapshot.TryGetBool(
            id,
            value))
    {
        ADD_FAILURE()
            << "bool not found";

        return false;
    }

    return value;
}

inline std::int32_t GetInt32(
    const RIMSnapshot& snapshot,
    RIDataId id)
{
    std::int32_t value{};

    if (!snapshot.TryGetInt32(
            id,
            value))
    {
        ADD_FAILURE()
            << "int32 not found";

        return {};
    }

    return value;
}

} // namespace rim