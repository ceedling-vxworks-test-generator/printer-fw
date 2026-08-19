#pragma once

#include "RIId.hpp"

namespace rim
{

struct CapabilityInput
{
    RIDataId changedDataId
    {
        RI_INVALID_DATA_ID
    };

    RIDataId CompressionKey() const
    {
        return changedDataId;
    }
};

} // namespace rim