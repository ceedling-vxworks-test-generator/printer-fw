#pragma once

#include <array>

#include "CapabilityItemDefinition.hpp"

namespace rim
{

inline constexpr std::array
kEnvironmentItems =
{
    std::string_view("TemperatureSensorA"),
    std::string_view("TemperatureSensorB"),
    std::string_view("HumiditySensor")
};

inline constexpr CapabilityItemDefinition
kEnvironmentCapability
{
    "Environment",
    kEnvironmentItems.data(),
    kEnvironmentItems.size()
};

inline constexpr std::array
kPrintReadyItems =
{
    std::string_view("UpperDoorOpen"),
    std::string_view("RightDoorOpen"),
    std::string_view("LeftDoorOpen"),
    // std::string_view("ErrorList")
};

inline constexpr CapabilityItemDefinition
kPrintReadyCapability
{
    "PrintReady",
    kPrintReadyItems.data(),
    kPrintReadyItems.size()
};

inline constexpr std::array
kConsumableItems =
{
    std::string_view("StapleLevel"),
    std::string_view("TonerLevel")
};

inline constexpr CapabilityItemDefinition
kConsumableCapability
{
    "Consumable",
    kConsumableItems.data(),
    kConsumableItems.size()
};

inline constexpr std::array
kJobItems =
{
    std::string_view("JobActive"),
    std::string_view("JobId")
};

inline constexpr CapabilityItemDefinition
kJobCapability
{
    "Job",
    kJobItems.data(),
    kJobItems.size()
};

inline constexpr std::array
kErrorItems =
{
    std::string_view("ErrorList")
};

inline constexpr CapabilityItemDefinition
kErrorCapability
{
    "Error",
    nullptr,
    0
};

}