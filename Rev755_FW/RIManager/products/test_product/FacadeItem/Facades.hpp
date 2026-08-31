#pragma once

#include <array>

#include "../facade_id.h"
#include "../capability_id.h"
#include "FacadeItemDefinition.hpp"
#include "FacadeBuilders.hpp"
#include "FacadeComparators.hpp"

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

inline constexpr FacadeItemDefinition kEnvironmentReadyFacade   {   RI_FACADE_ENVIRONMENT_READY,    "EnvironmentReady",     ValueType::kBool,   {    ValueType::kBool,    { .b = false },    0  }   ,   BuildEnvironmentReadyFacade,    IdentityDiff,   kEnvironmentReadyItems.data(),  kEnvironmentReadyItems.size(),  CompressionPolicy::KeepOldest,  100  };
inline constexpr FacadeItemDefinition kOperationReadyFacade     {   RI_FACADE_OPERATION_READY,      "OperationReady",       ValueType::kBool,   {    ValueType::kBool,    { .b = false },    0  }   ,   BuildOperationReadyFacade,      IdentityDiff,   kOperationReadyItems.data(),    kOperationReadyItems.size(),    CompressionPolicy::KeepOldest,  100  };

} // namespace rim