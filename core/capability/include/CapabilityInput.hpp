#pragma once

#include "RIMSnapshot.hpp"

namespace rim
{

struct CapabilityInput
{
    RIMSnapshot snapshot;

    RIDataId changedDataId{
        RI_INVALID_DATA_ID
    };

    RIDataId CompressionKey() const
    {
        return changedDataId;
    }
};

}