#pragma once

#include "datastore/SensorId.hpp"

#include "datastore/RIMDataId.hpp"
#include "datastore/ValueType.hpp"

namespace rim
{

struct SensorDefinition
{
    SensorId sensorId;

    RIMDataId dataId;

    ValueType valueType;
};

} // namespace rim