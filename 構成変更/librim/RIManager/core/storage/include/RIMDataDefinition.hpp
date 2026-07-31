#pragma once

#include "DataDomain.hpp"
#include "RIMDataId.hpp"
#include "ValueType.hpp"

namespace rim
{

struct RIMDataDefinition
{
    RIMDataId id;
    DataDomain domain;
    ValueType valueType;
    const char* name;
};

} // namespace rim