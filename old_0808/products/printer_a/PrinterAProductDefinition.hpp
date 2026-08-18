#pragma once

#include <array>

#include "DataItemDefinition.hpp"
#include "DataItem/PrinterADataItems.hpp"

#include "DomainDefinition.hpp"
#include "Domain/PrinterADomains.hpp"

#include "CapabilityItem/CapabilityItemDefinition.hpp"
#include "CapabilityItem/PrinterACapabilities.hpp"

#include "Pipeline/PipelineDefinition.hpp"
#include "Pipeline/PrinterAPipelines.hpp"

#include "ProductDefinition.hpp"

namespace rim
{

inline constexpr std::array kDomains =
{
    kEnvironmentDomain,
    kDoorDomain,
    // kConsumableDomain,
    // kJobDomain,
};

inline constexpr std::array kRoutes =
{
    kValue,
    kData,
    kError
};

inline constexpr std::array kDataItems =
{
    kTemperatureSensorA,
    kTemperatureSensorB,
    kHumiditySensor,

    kUpperDoorOpen,
    kRightDoorOpen,
    kLeftDoorOpen,

    kStapleLevel,
    kTonerLevel,

    kJobActive,
    kJobId,

    kErrorList
};

inline constexpr std::array kCapabilities =
{
    kEnvironmentCapability,
    kPrintReadyCapability,
    // kConsumableCapability,
    // kJobCapability,
};

inline constexpr std::array kPipelines =
{
    kEnvironmentPipeline,
    kPrintReadyPipeline,
    // kConsumablePipeline,
    // kJobPipeline,
};

inline constexpr ProductDefinition
kPrinterAProductDefinition
{
    kDomains.data(),
    kDomains.size(),

    kRoutes.data(),
    kRoutes.size(),

    kDataItems.data(),
    kDataItems.size(),

    kCapabilities.data(),
    kCapabilities.size(),

    kPipelines.data(),
    kPipelines.size()
};

}