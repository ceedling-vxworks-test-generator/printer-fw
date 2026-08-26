#pragma once

#include <string_view>

#include "RIId.hpp"
#include "ValueType.hpp"
#include "RIMValue.hpp"
#include "DomainDefinition.hpp"

namespace rim
{

class RIMValue;

using NormalizeCallback =
    RIMValue (*)(
        const RIMValue& value,
        const void* context);

using StoreCallback =
    RIMValue (*)(
        const RIMValue& currentValue,
        const RIMValue& setValue,
        const void* context);

using DiffCallback =
    bool (*)(
        const RIMValue& lhs,
        const RIMValue& rhs);

struct DataItemDefinition
{
    RIDataId id;

    std::string_view name;
    // std::string_view description;

    const DomainDefinition* domain;
    std::uint32_t routeId;
    // std::string_view route;

    // ValueType valueType;
    ValueType rawValueType;
    ValueType setValueType;
    ValueType storeValueType;
    ValueType getValueType;
    // std::string_view unit;

    RIMValue initialValue;

    NormalizeCallback normalize;
    StoreCallback store;
    DiffCallback diff;
    // SetCallback set;
    // GetCallback get;

    // DataItemFlags flags;

    // DataSourceType sourceType;

};

} // namespace rim
