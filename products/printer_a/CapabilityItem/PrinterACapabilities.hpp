#pragma once

#include <array>

#include "../printer_a_capability_id.h"
#include "../printer_a_data_id.h"
#include "CapabilityItemDefinition.hpp"
#include "PrinterACapabilityBuilders.hpp"
#include "PrinterACapabilityComparators.hpp"

namespace rim
{

inline constexpr std::array<RIDataId, 3>
kEnvironmentItems =
{
    RI_DATA_TEMPERATURE_SENSOR_A,
    RI_DATA_TEMPERATURE_SENSOR_B,
    RI_DATA_HUMIDITY_SENSOR
};

inline constexpr std::array<RIDataId, 3>
kPrintReadyItems =
{
    RI_DATA_UPPER_DOOR_OPEN,
    RI_DATA_RIGHT_DOOR_OPEN,
    RI_DATA_LEFT_DOOR_OPEN
};
inline constexpr CapabilityItemDefinition kEnvironmentCapability    {   RI_CAPABILITY_ENVIRONMENT,  "Environment",  ValueType::kInt32,  {   ValueType::kInt32,  {   .i32 = false  },    0   },  BuildEnvironmentCapability, IdentityDiff, kEnvironmentItems.data(),   kEnvironmentItems.size(),   ROUTE_VALUE    };
inline constexpr CapabilityItemDefinition kPrintReadyCapability     {   RI_CAPABILITY_PRINT_READY,  "PrintReady",   ValueType::kBool,   {   ValueType::kBool,   {   .b = false  },      0   },  BuildPrintReadyCapability,  IdentityDiff,  kPrintReadyItems.data(),    kPrintReadyItems.size(),    ROUTE_DATA     };
} // namespace rim