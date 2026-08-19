#include <gtest/gtest.h>

#include "SnapshotAccessor.hpp"

#include "test/support/AccessorTestConstants.hpp"
#include "test/support/AccessorTestFixture.hpp"

namespace
{

class SnapshotAccessorTest :
    public ::testing::Test,
    protected rim::test::AccessorTestFixture
{
protected:

    SnapshotAccessorTest()
        :
        accessor(
            domainStore,
            snapshotStorage,
            rim::kPrinterAProductDefinition)
    {
    }

    rim::RIMSnapshot
    GetSnapshot(
        rim::AccessId accessId)
    {
        rim::RIMSnapshot snapshot;

        EXPECT_TRUE(
            accessor.TryGetSnapshot(
                accessId,
                snapshot));

        return snapshot;
    }

    rim::SnapshotAccessor accessor;
};

} // namespace

TEST_F(
    SnapshotAccessorTest,
    GetCreatedSnapshot)
{
    StoreTemperatureA();

    const auto result =
        accessor.CreateSnapshot(
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_GT(
        result.accessId,
        0u);

    const auto snapshot =
        GetSnapshot(
            result.accessId);

    ASSERT_EQ(
        snapshot.items.size(),
        1u);

    double value{};

    ASSERT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        rim::test::kTemperatureA);
}

TEST_F(
    SnapshotAccessorTest,
    CreateSnapshotFromMultipleDataIds)
{
    StoreTemperatureA();
    StoreHumidity();

    const auto result =
        accessor.CreateSnapshot(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                RI_DATA_HUMIDITY_SENSOR
            });

    const auto snapshot =
        GetSnapshot(
            result.accessId);

    ASSERT_EQ(
        snapshot.items.size(),
        2u);

    double temperature{};
    double humidity{};

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            temperature));

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            humidity));

    EXPECT_DOUBLE_EQ(
        temperature,
        rim::test::kTemperatureA);

    EXPECT_DOUBLE_EQ(
        humidity,
        rim::test::kHumidity);
}

TEST_F(
    SnapshotAccessorTest,
    IgnoreUnknownDataIdAndKeepValidData)
{
    StoreTemperatureA();

    const auto result =
        accessor.CreateSnapshot(
            {
                rim::test::kUnknownDataId,
                RI_DATA_TEMPERATURE_SENSOR_A
            });

    const auto snapshot =
        GetSnapshot(
            result.accessId);

    ASSERT_EQ(
        snapshot.items.size(),
        1u);

    double value{};

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        rim::test::kTemperatureA);
}

TEST_F(
    SnapshotAccessorTest,
    AccessIdIncreases)
{
    StoreTemperatureA();

    const auto result1 =
        accessor.CreateSnapshot(
            RI_DATA_TEMPERATURE_SENSOR_A);

    const auto result2 =
        accessor.CreateSnapshot(
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_GT(
        result2.accessId,
        result1.accessId);
}

TEST_F(
    SnapshotAccessorTest,
    ReturnFalseWhenSnapshotDoesNotExist)
{
    rim::RIMSnapshot snapshot;

    EXPECT_FALSE(
        accessor.TryGetSnapshot(
            rim::test::kUnknownAccessId,
            snapshot));
}