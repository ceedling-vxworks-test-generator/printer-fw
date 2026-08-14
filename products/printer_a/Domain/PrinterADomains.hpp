#pragma once

#include "DataAccessor.hpp"

#include "DataItemDefinition.hpp"
#include "DomainDefinition.hpp"

namespace rim
{

inline constexpr DomainDefinition
kEnvironmentDomain
{
    "Environment"
};

inline constexpr DomainDefinition
kDoorDomain
{
    "Door"
};

inline constexpr DomainDefinition
kConsumableDomain
{
    "Consumable"
};

inline constexpr DomainDefinition
kJobDomain
{
    "Job"
};

inline constexpr DomainDefinition
kErrorDomain
{
    "Error"
};

}