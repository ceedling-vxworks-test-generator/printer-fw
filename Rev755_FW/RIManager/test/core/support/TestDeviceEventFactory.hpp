#pragma once

#include "DeviceEvent.hpp"
#include "SensorId.hpp"

namespace rim
{

class TestDeviceEventFactory
{
public:

    static DeviceEvent CreateTemperatureA(
        double value);

    static DeviceEvent CreateTemperatureB(
        double value);

    static DeviceEvent CreateHumidity(
        double value);

    static DeviceEvent CreateUpperDoor(
        bool opened);

    static DeviceEvent CreateRightDoor(
        bool opened);

    static DeviceEvent CreateLeftDoor(
        bool opened);
};

}