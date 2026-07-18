#include "adapter/AdapterFactory.hpp"

#include "adapter/DoorSensorAdapter.hpp"
#include "adapter/HumiditySensorAdapter.hpp"
#include "adapter/TemperatureSensorAAdapter.hpp"
#include "adapter/TemperatureSensorBAdapter.hpp"

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
                RIMDataId::kDoorUpper);

    case SensorId::kRightDoorSensor:

        return std::make_unique<
            DoorSensorAdapter>(
                RIMDataId::kDoorRight);

    case SensorId::kLeftDoorSensor:

        return std::make_unique<
            DoorSensorAdapter>(
                RIMDataId::kDoorLeft);
    }

    return nullptr;
}

} // namespace rim