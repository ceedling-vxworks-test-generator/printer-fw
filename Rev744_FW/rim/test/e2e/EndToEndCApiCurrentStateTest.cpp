#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "rim_api.h"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "ConsumableCapability.hpp"
#include "JobCapability.hpp"

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

    void WaitPipeline()
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
        RIM_TestInjectTemperature(
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        30.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CapabilityCanBeReadMultipleTimes)
{
    ASSERT_EQ(
        RIM_TestInjectTemperature(
            35.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        first{};

    rim::EnvironmentCapability
        second{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &first),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &second),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        first.temperature,
        35.0);

    EXPECT_DOUBLE_EQ(
        second.temperature,
        35.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CapabilityReturnsLatestEnvironmentValue)
{
    ASSERT_EQ(
        RIM_TestInjectTemperature(
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    ASSERT_EQ(
        RIM_TestInjectTemperature(
            40.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_ENVIRONMENT,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        40.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetPrintReady)
{
    ASSERT_EQ(
        RIM_TestInjectUpperDoorOpen(
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_TestInjectRightDoorOpen(
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_TestInjectLeftDoorOpen(
            0),
        RI_SUCCESS);

    WaitPipeline();

    rim::PrintReadyCapability
        capability{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_PRINT_READY,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetConsumable)
{
    ASSERT_EQ(
        RIM_TestInjectStapleLevel(
            80),
        RI_SUCCESS);

    WaitPipeline();

    rim::ConsumableCapability
        capability{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_CONSUMABLE,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability.stapleLevel,
        80);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetJob)
{
    ASSERT_EQ(
        RIM_TestInjectJobActive(
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_TestInjectJobId(
            123),
        RI_SUCCESS);

    WaitPipeline();

    rim::JobCapability
        capability{};

    ASSERT_EQ(
        RIM_GetCapability(
            RI_CAPABILITY_JOB,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.jobActive);

    EXPECT_EQ(
        capability.jobId,
        123);
}