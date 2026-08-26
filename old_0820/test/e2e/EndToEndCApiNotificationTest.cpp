#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "printer_a.h"

#include "EnvironmentCapability.hpp"

TEST(
    EndToEndCApiNotificationTest,
    StartStopAndReceiveCapability)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

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

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}