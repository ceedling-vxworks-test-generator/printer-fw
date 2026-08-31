#pragma once

#include <array>

#include "DataItemDefinition.hpp"
#include "DataItem/DataItems.hpp"

#include "DomainDefinition.hpp"
#include "Domain/Domains.hpp"

#include "CapabilityItemDefinition.hpp"
#include "CapabilityItem/Capabilities.hpp"
#include "FacadeItem/Facades.hpp"

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

inline constexpr std::array kRoutes =
{
    kValueRoute,
    kDataRoute,
    kErrorRoute

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
};

inline constexpr std::array kFacades =
{
    kEnvironmentReadyFacade,
    kOperationReadyFacade,
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

    kFacades.data(),
    kFacades.size(),
};

}