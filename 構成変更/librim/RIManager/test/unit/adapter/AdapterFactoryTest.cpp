#include <gtest/gtest.h>

#include "AdapterFactory.hpp"

TEST(
    AdapterFactoryTest,
    CreateTemperatureSensorA)
{
    auto adapter =
        rim::AdapterFactory::Create(
            rim::SensorId::kTemperatureSensorA);

    ASSERT_NE(
        nullptr,
        adapter);
}

TEST(
    AdapterFactoryTest,
    CreateUpperDoorSensor)
{
    auto adapter =
        rim::AdapterFactory::Create(
            rim::SensorId::kUpperDoorSensor);

    ASSERT_NE(
        nullptr,
        adapter);
}