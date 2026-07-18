#include "datastore/PrinterAProvider.hpp"

#include <vector>

#include "datastore/SensorDefinition.hpp"

namespace rim

{

    const std::vector<RIMDataDefinition>&
PrinterAProvider::GetDefinitions() const
{
    static const std::vector<RIMDataDefinition>
    definitions =
    {
        {
            RIMDataId::kTemperature,
            DataDomain::kDevice,
            ValueType::kDouble,
            "Temperature"
        },

        {
            RIMDataId::kHumidify,
            DataDomain::kDevice,
            ValueType::kDouble,
            "Humidify"
        },

        {
            RIMDataId::kDoorUpper,
            DataDomain::kDevice,
            ValueType::kBool,
            "DoorUpper"
        },

        {
            RIMDataId::kDoorRight,
            DataDomain::kDevice,
            ValueType::kBool,
            "DoorRight"
        },

        {
            RIMDataId::kDoorLeft,
            DataDomain::kDevice,
            ValueType::kBool,
            "DoorLeft"
        },

        {
            RIMDataId::kCurrentError,
            DataDomain::kError,
            ValueType::kStruct,
            "CurrentError"
        },

        {
            RIMDataId::kStapleLevel,
            DataDomain::kConsumable,
            ValueType::kUInt32,
            "StapleLevel"
        }
    };

    return definitions;
}

const std::vector<SensorDefinition>&
PrinterAProvider::GetSensorDefinitions() const
{
    static const std::vector<SensorDefinition>
    definitions =
    {
        {
            SensorId::kTemperatureSensorA,
            RIMDataId::kTemperature,
            ValueType::kDouble
        },

        {
            SensorId::kTemperatureSensorB,
            RIMDataId::kTemperature,
            ValueType::kDouble
        },

        {
            SensorId::kHumiditySensor,
            RIMDataId::kHumidify,
            ValueType::kDouble
        },

        {
            SensorId::kUpperDoorSensor,
            RIMDataId::kDoorUpper,
            ValueType::kBool
        },

        {
            SensorId::kRightDoorSensor,
            RIMDataId::kDoorRight,
            ValueType::kBool
        },

        {
            SensorId::kLeftDoorSensor,
            RIMDataId::kDoorLeft,
            ValueType::kBool
        }
    };

    return definitions;
}

} // namespace rim