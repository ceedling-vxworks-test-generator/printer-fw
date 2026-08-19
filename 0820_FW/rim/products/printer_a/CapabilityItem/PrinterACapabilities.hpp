#pragma once

#include <array>

#include "CapabilityItemDefinition.hpp"
#include "PrinterACapabilityBuilders.hpp"
#include "RIMDataId.hpp"

namespace rim
{

inline constexpr std::array<RIMDataId, 3>
kEnvironmentItems =
{
    RIMDataId::kTemperatureSensorA,
    RIMDataId::kTemperatureSensorB,
    RIMDataId::kHumiditySensor
};

inline constexpr CapabilityItemDefinition
kEnvironmentCapability
{
    RI_CAPABILITY_ENVIRONMENT,

    "Environment",

    BuildEnvironmentCapability,

    kEnvironmentItems.data(),
    kEnvironmentItems.size()
};

inline constexpr std::array<RIMDataId, 3>
kPrintReadyItems =
{
    RIMDataId::kUpperDoorOpen,
    RIMDataId::kRightDoorOpen,
    RIMDataId::kLeftDoorOpen,
};

inline constexpr CapabilityItemDefinition
kPrintReadyCapability
{
    RI_CAPABILITY_PRINT_READY,

    "PrintReady",

    BuildPrintReadyCapability,

    kPrintReadyItems.data(),
    kPrintReadyItems.size()
};

inline constexpr std::array<RIMDataId, 2>
kConsumableItems =
{
    RIMDataId::kStapleLevel,
    RIMDataId::kTonerLevel
};

inline constexpr CapabilityItemDefinition
kConsumableCapability
{
    RI_CAPABILITY_CONSUMABLE,

    "Consumable",

    BuildConsumableCapability,

    kConsumableItems.data(),
    kConsumableItems.size()
};

inline constexpr std::array<RIMDataId, 2>
kJobItems =
{
    RIMDataId::kJobActive,
    RIMDataId::kJobId
};

inline constexpr CapabilityItemDefinition
kJobCapability
{
    RI_CAPABILITY_JOB,

    "Job",

    BuildJobCapability,

    kJobItems.data(),
    kJobItems.size()
};

}