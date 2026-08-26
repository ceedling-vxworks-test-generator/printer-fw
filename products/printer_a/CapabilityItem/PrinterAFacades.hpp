#pragma once

#include <array>

#include "../printer_a_facade_id.h"
#include "../printer_a_capability_id.h"
#include "FacadeItemDefinition.hpp"
#include "PrinterAFacadeBuilders.hpp"
#include "PrinterACapabilityComparators.hpp"

namespace rim
{

inline constexpr std::array<RICapabilityId, 1>
kEnvironmentReadyItems =
{
    RI_CAPABILITY_ENVIRONMENT
};

inline constexpr std::array<RICapabilityId, 2>
kOperationReadyItems =
{
    RI_CAPABILITY_ENVIRONMENT,
    RI_CAPABILITY_PRINT_READY
};

inline constexpr FacadeItemDefinition kEnvironmentReadyFacade   {   RI_FACADE_ENVIRONMENT_READY,    "EnvironmentReady",     ValueType::kBool,   {    ValueType::kBool,    { .b = false },    0  }   ,   BuildEnvironmentReadyFacade,    IdentityDiff,   kEnvironmentReadyItems.data(),  kEnvironmentReadyItems.size(),  ROUTE_FACADE   };
inline constexpr FacadeItemDefinition kOperationReadyFacade     {   RI_FACADE_OPERATION_READY,      "OperationReady",       ValueType::kBool,   {    ValueType::kBool,    { .b = false },    0  }   ,   BuildOperationReadyFacade,      IdentityDiff,   kOperationReadyItems.data(),    kOperationReadyItems.size(),    ROUTE_FACADE   };

} // namespace rim