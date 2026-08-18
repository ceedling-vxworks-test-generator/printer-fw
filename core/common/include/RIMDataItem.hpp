#pragma once

#include "RIId.hpp"
#include "ValueType.hpp"
#include "RIMValue.hpp"

namespace rim
{

struct RIMDataItem
{
    RIDataId id;

    ValueType valueType;

    RIMValue value;

    RIDataId CompressionKey() const
    {
        return id;
    }
};

} // namespace rim