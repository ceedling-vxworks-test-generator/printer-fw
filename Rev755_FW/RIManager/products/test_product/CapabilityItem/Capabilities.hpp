#pragma once

#include <array>

#include "capability_id.h"
#include "data_id.h"
#include "CapabilityItemDefinition.hpp"
#include "CapabilityBuilders.hpp"
#include "CapabilityComparators.hpp"

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
inline constexpr CapabilityItemDefinition kEnvironmentCapability    {   RI_CAPABILITY_ENVIRONMENT,  "Environment",  ValueType::kInt32,  {   ValueType::kInt32,  {   .i32 = false  },    0   },  BuildEnvironmentCapability, IdentityDiff, kEnvironmentItems.data(),   kEnvironmentItems.size(),   CompressionPolicy::KeepLatest,  100  };
inline constexpr CapabilityItemDefinition kPrintReadyCapability     {   RI_CAPABILITY_PRINT_READY,  "PrintReady",   ValueType::kBool,   {   ValueType::kBool,   {   .b = false  },      0   },  BuildPrintReadyCapability,  IdentityDiff,  kPrintReadyItems.data(),    kPrintReadyItems.size(),   CompressionPolicy::KeepOldest,  100  };
} // namespace rim