#pragma once

#include "rim_data_id.h"

#include "RIMSnapshot.hpp"

namespace rim
{

struct CapabilityInput
{
    RIMSnapshot snapshot;

    RIDataId changedDataId{
        RI_DATA_UNKNOWN
    };
};

}