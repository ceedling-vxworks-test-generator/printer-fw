#pragma once

#include "SensorId.hpp"

#include "RIMDataId.hpp"
#include "ValueType.hpp"

namespace rim
{

struct SensorDefinition
{
    SensorId sensorId;

    RIMDataId dataId;

    ValueType valueType;
};

} // namespace rim