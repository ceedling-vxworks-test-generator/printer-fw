#pragma once

#include "rim_data_id.h"
#include "ValueType.hpp"
#include "RIMValue.hpp"

namespace rim
{

struct RIMDataItem
{
    RIDataId id;

    ValueType valueType;

    RIMValue value;
};

} // namespace rim