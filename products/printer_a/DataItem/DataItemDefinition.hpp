#pragma once

#include <string_view>

#include "ValueType.hpp"
#include "RIMDataId.hpp"

namespace rim
{

struct DataItemDefinition
{
    RIMDataId id;
    
    std::string_view name;

    std::string_view domain;

    ValueType valueType;
};

}