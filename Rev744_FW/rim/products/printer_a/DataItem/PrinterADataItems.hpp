#pragma once

#include "DataItemDefinition.hpp"

#include "../Domain/PrinterADomains.hpp"

#include "PrinterANormalizationFunctions.hpp"

#include "../../common/CommonStoreFunctions.hpp"
#include "../../common/CommonDiffFunctions.hpp"

namespace rim
{

inline constexpr DataItemDefinition
kTemperatureSensorA
{
    RIMDataId::kTemperatureSensorA,
    "TemperatureSensorA",
    kEnvironmentDomain.name,

    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,

    NormalizeTemperatureSensorA,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kTemperatureSensorB
{
    RIMDataId::kTemperatureSensorB,
    "TemperatureSensorB",
    kEnvironmentDomain.name,

    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kHumiditySensor
{
    RIMDataId::kHumiditySensor,
    "HumiditySensor",
    kEnvironmentDomain.name,

    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,
    ValueType::kDouble,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kUpperDoorOpen
{
    RIMDataId::kUpperDoorOpen,
    "UpperDoorOpen",
    kDoorDomain.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kRightDoorOpen
{
    RIMDataId::kRightDoorOpen,
    "RightDoorOpen",
    kDoorDomain.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kLeftDoorOpen
{
    RIMDataId::kLeftDoorOpen,
    "LeftDoorOpen",
    kDoorDomain.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kStapleLevel
{
    RIMDataId::kStapleLevel,
    "StapleLevel",
    kConsumableDomain.name,

    ValueType::kUInt32,
    ValueType::kUInt32,
    ValueType::kUInt32,
    ValueType::kUInt32,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kTonerLevel
{
    RIMDataId::kTonerLevel,
    "TonerLevel",
    kConsumableDomain.name,

    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kJobActive
{
    RIMDataId::kJobActive,
    "JobActive",
    kJobDomain.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kJobId
{
    RIMDataId::kJobId,
    "JobId",
    kJobDomain.name,

    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,

    IdentityNormalize,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kErrorList
{
    RIMDataId::kErrorList,
    "ErrorList",
    kErrorDomain.name,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    NormalizeBinary,

    IdentityStore,

    IdentityDiff
};

} // namespace rim