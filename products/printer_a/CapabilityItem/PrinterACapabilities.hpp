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

// inline constexpr std::array<
//     RIDataId,
//     2>
// kConsumableItems =
// {
//     RI_DATA_STAPLE_LEVEL,
//     RI_DATA_TONER_LEVEL
// };

// inline constexpr std::array<
//     RIDataId,
//     2>
// kJobItems =
// {
//     RI_DATA_JOB_ACTIVE,
//     RI_DATA_JOB_ID
// };

inline constexpr CapabilityItemDefinition kEnvironmentCapability    {   RI_CAPABILITY_ENVIRONMENT,  "Environment",  ValueType::kBool,   BuildEnvironmentCapability, CompareEnvironment, kEnvironmentItems.data(),   kEnvironmentItems.size(),	"ValueRoute"   };
inline constexpr CapabilityItemDefinition kPrintReadyCapability     {   RI_CAPABILITY_PRINT_READY,  "PrintReady",   ValueType::kBool,   BuildPrintReadyCapability,  ComparePrintReady,  kPrintReadyItems.data(),    kPrintReadyItems.size(),	"DataRoute"    };
// inline constexpr CapabilityItemDefinition kConsumableCapability     {   RI_CAPABILITY_CONSUMABLE,   "Consumable",   ValueType::kBool,   BuildConsumableCapability,  CompareConsumable,  kConsumableItems.data(),    kConsumableItems.size(),    "DataRoute"    };
// inline constexpr CapabilityItemDefinition kJobCapability            {   RI_CAPABILITY_JOB,          "Job",          ValueType::kBool,   BuildJobCapability,         CompareJob,         kJobItems.data(),           kJobItems.size(),           "ErrorRoute"   };

} // namespace rim