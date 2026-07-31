#pragma once

#include <array>

#include "DataItem/DataItemDefinition.hpp"
#include "DataItem/PrinterADataItems.hpp"

#include "Domain/DomainDefinition.hpp"
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
    kConsumableDomain,
    kJobDomain,
    kErrorDomain
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

    // kErrorList
};

inline constexpr std::array kCapabilities =
{
    kEnvironmentCapability,
    kPrintReadyCapability,
    kConsumableCapability,
    kJobCapability,
    kErrorCapability
};

inline constexpr std::array kPipelines =
{
    kEnvironmentPipeline,
    kPrintReadyPipeline,
    kConsumablePipeline,
    kJobPipeline,
    kErrorPipeline
};

// struct ProductDefinition
// {
//     const DomainDefinition* domains;
//     std::size_t domainCount;

//     const DataItemDefinition* dataItems;
//     std::size_t dataItemCount;

//     const CapabilityItemDefinition* capabilities;
//     std::size_t capabilityCount;

//     const PipelineDefinition* pipelines;
//     std::size_t pipelineCount;
// };

inline constexpr ProductDefinition
kPrinterAProductDefinition
{
    kDomains.data(),
    kDomains.size(),

    kDataItems.data(),
    kDataItems.size(),

    kCapabilities.data(),
    kCapabilities.size(),

    kPipelines.data(),
    kPipelines.size()
};

}