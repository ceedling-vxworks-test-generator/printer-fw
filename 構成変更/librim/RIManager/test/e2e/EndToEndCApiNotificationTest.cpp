#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "rim_api.h"

#include "EnvironmentCapability.hpp"

TEST(
    EndToEndCApiNotificationTest,
    StartStopAndReceiveCapability)
{
    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle,
            30.0),
        RI_SUCCESS);

    rim::EnvironmentCapability
        capability{};

    bool received = false;

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

    while (std::chrono::steady_clock::now()
           < timeout)
    {
        if (RIManager_GetEnvironment(
                handle,
                &capability)
            == RI_SUCCESS)
        {
            received = true;
            break;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        received);

    if (received)
    {
        EXPECT_DOUBLE_EQ(
            capability.temperature,
            30.0);
    }

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}