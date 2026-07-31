#pragma once

#include "DataItemDefinition.hpp"
#include "../Domain/PrinterADomains.hpp"

namespace rim
{

inline constexpr DataItemDefinition
kTemperatureSensorA
{
    RIMDataId::kTemperatureSensorA,
    "TemperatureSensorA",
    kEnvironmentDomain.name,
    ValueType::kDouble
};

inline constexpr DataItemDefinition
kTemperatureSensorB
{
    RIMDataId::kTemperatureSensorB,
    "TemperatureSensorB",
    kEnvironmentDomain.name,
    ValueType::kDouble
};

inline constexpr DataItemDefinition
kHumiditySensor
{
    RIMDataId::kHumiditySensor,
    "HumiditySensor",
    kEnvironmentDomain.name,
    ValueType::kDouble
};

inline constexpr DataItemDefinition
kUpperDoorOpen
{
    RIMDataId::kUpperDoorOpen,
    "UpperDoorOpen",
    kDoorDomain.name,
    ValueType::kBool
};

inline constexpr DataItemDefinition
kRightDoorOpen
{
    RIMDataId::kRightDoorOpen,
    "RightDoorOpen",
    kDoorDomain.name,
    ValueType::kBool
};

inline constexpr DataItemDefinition
kLeftDoorOpen
{
    RIMDataId::kLeftDoorOpen,
    "LeftDoorOpen",
    kDoorDomain.name,
    ValueType::kBool
};

inline constexpr DataItemDefinition
kStapleLevel
{
    RIMDataId::kStapleLevel,
    "StapleLevel",
    kConsumableDomain.name,
    ValueType::kUInt32
};

inline constexpr DataItemDefinition
kTonerLevel
{
    RIMDataId::kTonerLevel,
    "TonerLevel",
    kConsumableDomain.name,
    ValueType::kInt32
};

inline constexpr DataItemDefinition
kJobActive
{
    RIMDataId::kJobActive,
    "JobActive",
    kJobDomain.name,
    ValueType::kBool
};

inline constexpr DataItemDefinition
kJobId
{
    RIMDataId::kJobId,
    "JobId",
    kJobDomain.name,
    ValueType::kInt32
};

// inline constexpr DataItemDefinition
// kErrorList
// {
//     RIMDataId::kErrorList,
//     "ErrorList",
//     kErrorDomain.name,
//     ValueType::kObject
// };

}