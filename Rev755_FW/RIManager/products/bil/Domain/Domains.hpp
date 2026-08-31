#pragma once

#include "DomainDefinition.hpp"

// 製品要求定義書(Domainシート)に定義されたDomain一覧。

namespace rim
{

inline constexpr DomainDefinition
kMachineDomain
{
    "Machine"
};

inline constexpr DomainDefinition
kPrintJobDomain
{
    "PrintJob"
};

inline constexpr DomainDefinition
kConsumableDomain
{
    "Consumable"
};

}
