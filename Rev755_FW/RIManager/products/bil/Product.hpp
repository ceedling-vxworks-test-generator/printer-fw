#pragma once

#include <array>

#include "DataItemDefinition.hpp"
#include "DataItem/DataItems.hpp"

#include "DomainDefinition.hpp"
#include "Domain/Domains.hpp"

#include "CapabilityItemDefinition.hpp"
#include "CapabilityItem/Capabilities.hpp"

#include "ProductDefinition.hpp"

// 製品要求定義書はFacadeを定義していないため、本製品にFacadeは存在しない
// (ProductDefinition::facades/facadeCountはnullptr/0)。

namespace rim
{

inline constexpr std::array kDomains =
{
    kMachineDomain,
    kPrintJobDomain,
    kConsumableDomain
};

inline constexpr std::array kRoutes =
{
    kDataRoute
};

inline constexpr std::array kDataItems =
{
    kLayerInitState,
    kPrintJobInfoList,
    kUnitStateList,
    kUseCaseExecutionStatusList,
    kSubPouchSensorValueList,
    kFaultInfoList,
    kFunctionLimitList
};

inline constexpr std::array kCapabilities =
{
    kProductStateCapability,
    kSupplyStatusCapability,
    kPrintJobStatusCapability
};

inline constexpr ProductDefinition
kProductDefinition
{
    kDomains.data(),
    kDomains.size(),

    kRoutes.data(),
    kRoutes.size(),

    kDataItems.data(),
    kDataItems.size(),

    kCapabilities.data(),
    kCapabilities.size(),

    nullptr,
    0,
};

}
