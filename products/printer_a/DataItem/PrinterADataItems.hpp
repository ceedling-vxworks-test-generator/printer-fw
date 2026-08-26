#pragma once

#include "../printer_a_data_id.h"
#include "DataItemDefinition.hpp"

#include "../Domain/PrinterADomains.hpp"
#include "../Route/PrinterARoute.hpp"

#include "PrinterANormalizationFunctions.hpp"
#include "PrinterADataComparators.hpp"

#include "../../common/CommonStoreFunctions.hpp"
#include "../../common/CommonDiffFunctions.hpp"

namespace rim
{

inline constexpr DataItemDefinition kTemperatureSensorA {   RI_DATA_TEMPERATURE_SENSOR_A,   "TemperatureSensorA",   &kEnvironmentDomain,    ROUTE_VALUE,   ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, {   ValueType::kDouble, {   .d = 0.0            },  0   },  NormalizeTemperatureSensorA,    IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kTemperatureSensorB {   RI_DATA_TEMPERATURE_SENSOR_B,   "TemperatureSensorB",   &kEnvironmentDomain,    ROUTE_VALUE,   ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, {   ValueType::kDouble, {   .d = 0.0            },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kHumiditySensor     {   RI_DATA_HUMIDITY_SENSOR,        "HumiditySensor",       &kEnvironmentDomain,    ROUTE_VALUE,   ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, ValueType::kDouble, {   ValueType::kDouble, {   .d = 0.0            },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kUpperDoorOpen      {   RI_DATA_UPPER_DOOR_OPEN,        "UpperDoorOpen",        &kDoorDomain,           ROUTE_VALUE,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   {   ValueType::kBool,   {   .b = false          },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kRightDoorOpen      {   RI_DATA_RIGHT_DOOR_OPEN,        "RightDoorOpen",        &kDoorDomain,           ROUTE_VALUE,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   {   ValueType::kBool,   {   .b = false          },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kLeftDoorOpen       {   RI_DATA_LEFT_DOOR_OPEN,         "LeftDoorOpen",         &kDoorDomain,           ROUTE_VALUE,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   {   ValueType::kBool,   {   .b = false          },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kStapleLevel        {   RI_DATA_STAPLE_LEVEL,           "StapleLevel",          &kConsumableDomain,     ROUTE_DATA,    ValueType::kUInt32, ValueType::kUInt32, ValueType::kUInt32, ValueType::kUInt32, {   ValueType::kUInt32, {   .u32 = 0            },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kTonerLevel         {   RI_DATA_TONER_LEVEL,            "TonerLevel",           &kConsumableDomain,     ROUTE_DATA,    ValueType::kInt32,  ValueType::kInt32,  ValueType::kInt32,  ValueType::kInt32,  {   ValueType::kInt32,  {   .i32 = 0            },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kJobActive          {   RI_DATA_JOB_ACTIVE,             "JobActive",            &kJobDomain,            ROUTE_DATA,    ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   ValueType::kBool,   {   ValueType::kBool,   {   .b = false          },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kJobId              {   RI_DATA_JOB_ID,                 "JobId",                &kJobDomain,            ROUTE_DATA,    ValueType::kInt32,  ValueType::kInt32,  ValueType::kInt32,  ValueType::kInt32,  {   ValueType::kInt32,  {   .i32 = 0            },  0   },  IdentityNormalize,              IdentityStore,  IdentityDiff    };
inline constexpr DataItemDefinition kErrorList          {   RI_DATA_ERROR_LIST,             "ErrorList",            &kErrorDomain,          ROUTE_ERROR,   ValueType::kBinary, ValueType::kBinary, ValueType::kBinary, ValueType::kBinary, {   ValueType::kBinary, {   .bytes = nullptr    }, 10   },  NormalizeBinary,                IdentityStore,  IdentityDiff    };

} // namespace rim