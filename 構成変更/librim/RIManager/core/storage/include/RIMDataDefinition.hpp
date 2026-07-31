#pragma once

#include "RIMDataId.hpp"
#include "ValueType.hpp"

namespace rim
{

enum class DataDomain
{
    kDevice,
    kJob,
    kError,
    kConsumable,
    kMaintenance,
    kNetwork
};

struct RIMDataDefinition
{
    RIMDataId id;
    DataDomain domain;
    ValueType valueType;
    const char* name;
};

} // namespace rim