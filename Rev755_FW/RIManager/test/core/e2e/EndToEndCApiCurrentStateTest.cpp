#include <gtest/gtest.h>

#include "rim_api.h"

#include "test/core/support/EndToEndFixture.hpp"

TEST_F(
    EndToEndFixture,
    GetEnvironment)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            40.0),
        RI_SUCCESS);

    WaitRoute();

    int32_t capability{};

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability,
        1);
}

TEST_F(
    EndToEndFixture,
    CapabilityCanBeReadMultipleTimes)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            40.0),
        RI_SUCCESS);

    WaitRoute();

    int32_t first{};
    int32_t second{};

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &first),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &second),
        RI_SUCCESS);

    EXPECT_EQ(
        first,
        second);
}

TEST_F(
    EndToEndFixture,
    CapabilityReturnsLatestEnvironmentValue)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            40.0),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            70.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            70.0),
        RI_SUCCESS);

    WaitRoute();

    int32_t capability{};

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability,
        2);
}

TEST_F(
    EndToEndFixture,
    GetPrintReady)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_RIGHT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_LEFT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    WaitRoute();

    int capability{};

    ASSERT_EQ(
        RIM_GetCapabilityBool(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability);
}

TEST_F(
    EndToEndFixture,
    EnvironmentStateChanges)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            20.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            20.0),
        RI_SUCCESS);

    WaitRoute();

    int32_t capability{};

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability,
        0);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            40.0),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_GetCapabilityInt32(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability,
        1);
}

TEST_F(
    EndToEndFixture,
    PrintReadyBecomesFalseWhenDoorOpen)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_RIGHT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_LEFT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            true),
        RI_SUCCESS);

    WaitRoute();

    int capability{};

    ASSERT_EQ(
        RIM_GetCapabilityBool(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_FALSE(
        capability);
}

TEST_F(
    EndToEndFixture,
    CapabilityReturnsLatestPrintReadyValue)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_RIGHT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_LEFT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            true),
        RI_SUCCESS);

    WaitRoute();

    int capability{};

    ASSERT_EQ(
        RIM_GetCapabilityBool(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_FALSE(
        capability);
}

TEST_F(
    EndToEndFixture,
    PrintReadyReturnsToTrue)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            true),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_RIGHT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_LEFT_DOOR_OPEN,
            false),
        RI_SUCCESS);

    WaitRoute();

    int capability{};

    ASSERT_EQ(
        RIM_GetCapabilityBool(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability);
}