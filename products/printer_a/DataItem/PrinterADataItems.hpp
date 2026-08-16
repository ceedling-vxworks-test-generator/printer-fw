#pragma once

#include "DataItemDefinition.hpp"

#include "../Domain/PrinterADomains.hpp"
#include "../Route/PrinterARoute.hpp"

#include "PrinterANormalizationFunctions.hpp"

#include "../../common/CommonStoreFunctions.hpp"
#include "../../common/CommonDiffFunctions.hpp"

namespace rim
{

inline constexpr DataItemDefinition
kTemperatureSensorA
{
    RI_DATA_TEMPERATURE_SENSOR_A,
    "TemperatureSensorA",
    kEnvironmentDomain.name,
    kValue.name,

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
    RI_DATA_TEMPERATURE_SENSOR_B,
    "TemperatureSensorB",
    kEnvironmentDomain.name,
    kValue.name,

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
    RI_DATA_HUMIDITY_SENSOR,
    "HumiditySensor",
    kEnvironmentDomain.name,
    kValue.name,

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
    RI_DATA_UPPER_DOOR_OPEN,
    "UpperDoorOpen",
    kDoorDomain.name,
    kValue.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalizeBool,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kRightDoorOpen
{
    RI_DATA_RIGHT_DOOR_OPEN,
    "RightDoorOpen",
    kDoorDomain.name,
    kValue.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalizeBool,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kLeftDoorOpen
{
    RI_DATA_LEFT_DOOR_OPEN,
    "LeftDoorOpen",
    kDoorDomain.name,
    kValue.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalizeBool,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kStapleLevel
{
    RI_DATA_STAPLE_LEVEL,
    "StapleLevel",
    kConsumableDomain.name,
    kData.name,

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
    RI_DATA_TONER_LEVEL,
    "TonerLevel",
    kConsumableDomain.name,
    kData.name,

    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,

    IdentityNormalizeInt32,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kJobActive
{
    RI_DATA_JOB_ACTIVE,
    "JobActive",
    kJobDomain.name,
    kData.name,

    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,
    ValueType::kBool,

    IdentityNormalizeBool,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kJobId
{
    RI_DATA_JOB_ID,
    "JobId",
    kJobDomain.name,
    kData.name,

    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,
    ValueType::kInt32,

    IdentityNormalizeInt32,

    IdentityStore,

    IdentityDiff
};

inline constexpr DataItemDefinition
kErrorList
{
    RI_DATA_ERROR_LIST,
    "ErrorList",
    kErrorDomain.name,
    kError.name,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    NormalizeBinary,

    IdentityStore,

    IdentityDiff
};

} // namespace rim