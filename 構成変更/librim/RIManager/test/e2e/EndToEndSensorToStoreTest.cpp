#include <gtest/gtest.h>

#include "test/support/TestSensorProcessor.hpp"

#include "RIMValueAccessor.hpp"

TEST(
    EndToEndSensorToStoreTest,
    TemperatureSensorA)
{
    rim::ValueStore store;

    rim::TestSensorProcessor::
        ProcessTemperatureA(
            27.0,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kTemperatureSensorA,
            item));

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    EndToEndSensorToStoreTest,
    TemperatureSensorB)
{
    rim::ValueStore store;

    rim::TestSensorProcessor::
        ProcessTemperatureB(
            300.15,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kTemperatureSensorB,
            item));

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    EndToEndSensorToStoreTest,
    HumiditySensor)
{
    rim::ValueStore store;

    rim::TestSensorProcessor::
        ProcessHumidity(
            60.0,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kHumiditySensor,
            item));

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        60.0);
}

TEST(
    EndToEndSensorToStoreTest,
    UpperDoorOpen)
{
    rim::ValueStore store;

    auto adapter =
        rim::AdapterFactory::Create(
            rim::SensorId::kUpperDoorSensor);

    adapter->Store(
        rim::RIMValueFactory::CreateBool(
            true),
        store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kUpperDoorOpen,
            item));

    bool opened{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            item.value,
            opened));

    EXPECT_TRUE(
        opened);
}

