#pragma once

#include "AdapterFactory.hpp"
#include "RIMValueFactory.hpp"
#include "ValueStore.hpp"

namespace rim
{

class TestSensorProcessor
{
public:

    static void ProcessTemperatureA(
        double value,
        ValueStore& store)
    {
        auto adapter =
            AdapterFactory::Create(
                SensorId::kTemperatureSensorA);

        adapter->Store(
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessTemperatureB(
        double value,
        ValueStore& store)
    {
        auto adapter =
            AdapterFactory::Create(
                SensorId::kTemperatureSensorB);

        adapter->Store(
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessHumidity(
        double value,
        ValueStore& store)
    {
        auto adapter =
            AdapterFactory::Create(
                SensorId::kHumiditySensor);

        adapter->Store(
            RIMValueFactory::CreateDouble(
                value),
            store);
    }
};

}
