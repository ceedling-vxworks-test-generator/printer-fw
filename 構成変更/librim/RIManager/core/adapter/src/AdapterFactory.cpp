#include "AdapterFactory.hpp"

#include "DoorSensorAdapter.hpp"
#include "HumiditySensorAdapter.hpp"
#include "TemperatureSensorAAdapter.hpp"
#include "TemperatureSensorBAdapter.hpp"

namespace rim
{

std::unique_ptr<ISensorAdapter>
AdapterFactory::Create(
    SensorId sensorId)
{
    switch (sensorId)
    {
    case SensorId::kTemperatureSensorA:

        return std::make_unique<
            TemperatureSensorAAdapter>();

    case SensorId::kTemperatureSensorB:

        return std::make_unique<
            TemperatureSensorBAdapter>();

    case SensorId::kHumiditySensor:

        return std::make_unique<
            HumiditySensorAdapter>();

    case SensorId::kUpperDoorSensor:

        return std::make_unique<
            DoorSensorAdapter>(
                RIMDataId::kUpperDoorOpen);

    case SensorId::kRightDoorSensor:

        return std::make_unique<
            DoorSensorAdapter>(
                RIMDataId::kRightDoorOpen);

    case SensorId::kLeftDoorSensor:

        return std::make_unique<
            DoorSensorAdapter>(
                RIMDataId::kLeftDoorOpen);
    }

    return nullptr;
}

} // namespace rim