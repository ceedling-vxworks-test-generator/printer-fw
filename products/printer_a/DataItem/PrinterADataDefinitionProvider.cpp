#include "PrinterADataDefinitionProvider.hpp"

#include <vector>

#include "SensorDefinition.hpp"

namespace rim

{

    const std::vector<RIMDataDefinition>&
PrinterADataDefinitionProvider::GetDefinitions() const
{
    static const std::vector<RIMDataDefinition>
    definitions =
    {
        {
            RIMDataId::kTemperatureSensorA,
            DataDomain::kDevice,
            ValueType::kDouble,
            "TemperatureSensorA"
        },
        {
            RIMDataId::kTemperatureSensorB,
            DataDomain::kDevice,
            ValueType::kDouble,
            "TemperatureSensorB"
        },

        {
            RIMDataId::kHumiditySensor,
            DataDomain::kDevice,
            ValueType::kDouble,
            "HumiditySensor"
        },

        {
            RIMDataId::kUpperDoorOpen,
            DataDomain::kDevice,
            ValueType::kBool,
            "UpperDoorOpen"
        },

        {
            RIMDataId::kRightDoorOpen,
            DataDomain::kDevice,
            ValueType::kBool,
            "RightDoorOpen"
        },

        {
            RIMDataId::kLeftDoorOpen,
            DataDomain::kDevice,
            ValueType::kBool,
            "LeftDoorOpen"
        },

        {
            RIMDataId::kStapleLevel,
            DataDomain::kConsumable,
            ValueType::kUInt32,
            "StapleLevel"
        },

        {
            RIMDataId::kTonerLevel,
            DataDomain::kConsumable,
            ValueType::kInt32,
            "TonerLevel"
        },

        {
            RIMDataId::kJobActive,
            DataDomain::kJob,
            ValueType::kBool,
            "JobActive"
        },

        {
            RIMDataId::kJobId,
            DataDomain::kJob,
            ValueType::kInt32,
            "JobId"
        },

        // {
        //     RIMDataId::kErrorList,
        //     DataDomain::kError,
        //     ValueType::kObject,
        //     "ErrorList"
        // },

    };

    return definitions;
}

const std::vector<SensorDefinition>&
PrinterADataDefinitionProvider::GetSensorDefinitions() const
{
    static const std::vector<SensorDefinition>
    definitions =
    {
        {
            SensorId::kTemperatureSensorA,
            RIMDataId::kTemperatureSensorA,
            ValueType::kDouble
        },

        {
            SensorId::kTemperatureSensorB,
            RIMDataId::kTemperatureSensorB,
            ValueType::kDouble
        },

        {
            SensorId::kHumiditySensor,
            RIMDataId::kHumiditySensor,
            ValueType::kDouble
        },

        {
            SensorId::kUpperDoorSensor,
            RIMDataId::kUpperDoorOpen,
            ValueType::kBool
        },

        {
            SensorId::kRightDoorSensor,
            RIMDataId::kRightDoorOpen,
            ValueType::kBool
        },

        {
            SensorId::kLeftDoorSensor,
            RIMDataId::kLeftDoorOpen,
            ValueType::kBool
        }
    };

    return definitions;
}

} // namespace rim