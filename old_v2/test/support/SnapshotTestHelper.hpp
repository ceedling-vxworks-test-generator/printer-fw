#pragma once

#include <cstdint>
#include <stdexcept>

#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

inline void AddDoubleItem(
    RIMSnapshot& snapshot,
    RIMDataId id,
    double value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kDouble;

    item.value =
        RIMValueFactory::CreateDouble(
            value);

    snapshot.items.PushBack(item);
}

inline void AddBoolItem(
    RIMSnapshot& snapshot,
    RIMDataId id,
    bool value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kBool;

    item.value =
        RIMValueFactory::CreateBool(
            value);

    snapshot.items.PushBack(item);
}

inline void AddInt32Item(
    RIMSnapshot& snapshot,
    RIMDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;
    item.valueType =
        ValueType::kInt32;

    item.value =
        RIMValueFactory::CreateInt32(
            value);

    snapshot.items.PushBack(item);
}

inline double GetDouble(
    const RIMSnapshot& snapshot,
    RIMDataId id)
{
    double value{};

    if (!snapshot.TryGetDouble(
            id,
            value))
    {
        throw std::runtime_error(
            "double not found");
    }

    return value;
}

inline bool GetBool(
    const RIMSnapshot& snapshot,
    RIMDataId id)
{
    bool value{};

    if (!snapshot.TryGetBool(
            id,
            value))
    {
        throw std::runtime_error(
            "bool not found");
    }

    return value;
}

inline std::int32_t GetInt32(
    const RIMSnapshot& snapshot,
    RIMDataId id)
{
    std::int32_t value{};

    if (!snapshot.TryGetInt32(
            id,
            value))
    {
        throw std::runtime_error(
            "int32 not found");
    }

    return value;
}

} // namespace rim