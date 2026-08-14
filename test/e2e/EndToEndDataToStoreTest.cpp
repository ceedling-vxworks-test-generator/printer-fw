#include <gtest/gtest.h>

#include "test/support/TestDataProcessor.hpp"

#include "RIMValueAccessor.hpp"
#include "BinaryStoreValue.hpp"

TEST(
    EndToEndDataToStoreTest,
    TemperatureSensorA)
{
    rim::ValueStore store;

    rim::TestDataProcessor::
        ProcessTemperatureA(
            27.0,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
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
    EndToEndDataToStoreTest,
    TemperatureSensorB)
{
    rim::ValueStore store;

    rim::TestDataProcessor::
        ProcessTemperatureB(
            300.15,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_TEMPERATURE_SENSOR_B,
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
    EndToEndDataToStoreTest,
    HumiditySensor)
{
    rim::ValueStore store;

    rim::TestDataProcessor::
        ProcessHumidity(
            60.0,
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_HUMIDITY_SENSOR,
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
    EndToEndDataToStoreTest,
    UpperDoorOpen)
{
    rim::ValueStore store;

    rim::TestDataProcessor::
        StoreData(
            RI_DATA_UPPER_DOOR_OPEN,
            rim::RIMValueFactory::CreateBool(
                true),
            store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_UPPER_DOOR_OPEN,
            item));

    bool opened{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            item.value,
            opened));

    EXPECT_TRUE(
        opened);
}

