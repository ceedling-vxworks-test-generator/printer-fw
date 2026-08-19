#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "printer_a.h"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

namespace
{

constexpr auto kWaitTime =
    std::chrono::milliseconds(
        100);

class EndToEndCApiCurrentStateTest
    : public ::testing::Test
{
protected:

    void SetUp() override
    {
        ASSERT_EQ(
            RIM_Create(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIM_Start(),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIM_Stop(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIM_Destroy(),
            RI_SUCCESS);
    }

    void WaitRoute()
    {
        std::this_thread::sleep_for(
            kWaitTime);
    }
};

} // namespace

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetEnvironment)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    WaitRoute();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        30.0 + 273.15);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CapabilityCanBeReadMultipleTimes)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            35.0),
        RI_SUCCESS);

    WaitRoute();

    rim::EnvironmentCapability
        first{};

    rim::EnvironmentCapability
        second{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &first),
        RI_SUCCESS);

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &second),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        first.temperature,
        35.0 + 273.15);

    EXPECT_DOUBLE_EQ(
        second.temperature,
        35.0 + 273.15);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CapabilityReturnsLatestEnvironmentValue)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            40.0),
        RI_SUCCESS);

    WaitRoute();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        40.0 + 273.15);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetPrintReady)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_RIGHT_DOOR_OPEN,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_LEFT_DOOR_OPEN,
            0),
        RI_SUCCESS);

    WaitRoute();

    rim::PrintReadyCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.ready);
}

// TEST_F(
//     EndToEndCApiCurrentStateTest,
//     GetConsumable)
// {
//     ASSERT_EQ(
//         RIM_TestInjectStapleLevel(
//             80),
//         RI_SUCCESS);

//     WaitRoute();

//     rim::ConsumableCapability
//         capability{};

//     ASSERT_EQ(
//         PrinterA_GetCapability(
//             RI_CAPABILITY_CONSUMABLE,
//             &capability),
//         RI_SUCCESS);

//     EXPECT_EQ(
//         capability.stapleLevel,
//         80);
// }

// TEST_F(
//     EndToEndCApiCurrentStateTest,
//     GetJob)
// {
//     ASSERT_EQ(
//         RIM_TestInjectJobActive(
//             1),
//         RI_SUCCESS);

//     ASSERT_EQ(
//         RIM_TestInjectJobId(
//             123),
//         RI_SUCCESS);

//     WaitRoute();

//     rim::JobCapability
//         capability{};

//     ASSERT_EQ(
//         PrinterA_GetCapability(
//             RI_CAPABILITY_JOB,
//             &capability),
//         RI_SUCCESS);

//     EXPECT_TRUE(
//         capability.jobActive);

//     EXPECT_EQ(
//         capability.jobId,
//         123);
// }

TEST_F(
    EndToEndCApiCurrentStateTest,
    EnvironmentContainsExpectedValues)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            60.0),
        RI_SUCCESS);

    WaitRoute();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        30.0 + 273.15);

    EXPECT_DOUBLE_EQ(
        capability.humidity,
        60.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    PrintReadyBecomesFalseWhenDoorOpen)
{
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            1),
        RI_SUCCESS);

    WaitRoute();

    rim::PrintReadyCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_FALSE(
        capability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CapabilityReturnsLatestHumidityValue)
{
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            50.0),
        RI_SUCCESS);

    WaitRoute();

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            70.0),
        RI_SUCCESS);

    WaitRoute();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        PrinterA_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.humidity,
        70.0);
}