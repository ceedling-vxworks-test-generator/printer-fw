#pragma once

#include <array>

#include "PipelineDefinition.hpp"

namespace rim
{

inline constexpr std::array
kEnvironmentTriggerDomains =
{
    std::string_view("Environment")
};

inline constexpr PipelineDefinition
kEnvironmentPipeline
{
    "EnvironmentPipeline",
    kEnvironmentTriggerDomains.data(),
    kEnvironmentTriggerDomains.size(),
    "Environment"
};

inline constexpr std::array
kDoorTriggerDomains =
{
    std::string_view("Door")
};

inline constexpr std::array
kPrintReadyTriggerDomains =
{
    std::string_view("Door"),
    std::string_view("Error")
};

inline constexpr PipelineDefinition
kPrintReadyPipeline
{
    "PrintReadyPipeline",
    kPrintReadyTriggerDomains.data(),
    kPrintReadyTriggerDomains.size(),
    "PrintReady"
};

inline constexpr std::array
kConsumableTriggerDomains =
{
    std::string_view("Consumable")
};

inline constexpr PipelineDefinition
kConsumablePipeline
{
    "ConsumablePipeline",
    kConsumableTriggerDomains.data(),
    kConsumableTriggerDomains.size(),
    "Consumable"
};

inline constexpr std::array
kJobTriggerDomains =
{
    std::string_view("Job")
};

inline constexpr PipelineDefinition
kJobPipeline
{
    "JobPipeline",
    kJobTriggerDomains.data(),
    kJobTriggerDomains.size(),
    "Job"
};

inline constexpr std::array
kErrorTriggerDomains =
{
    std::string_view("Error")
};

inline constexpr PipelineDefinition
kErrorPipeline
{
    "ErrorPipeline",
    kErrorTriggerDomains.data(),
    kErrorTriggerDomains.size(),
    "Error"
};

}