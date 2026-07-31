#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "rim_api.h"

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
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
            RIManager_Create(
                &handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Start(
                handle_),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIManager_Stop(
                handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Destroy(
                handle_),
            RI_SUCCESS);
    }

    void WaitPipeline()
    {
        std::this_thread::sleep_for(
            kWaitTime);
    }

    RIM_HANDLE handle_{};
};

} // namespace


TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentEnvironment)
{
    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle_,
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetCurrentEnvironment(
            handle_,
            &capability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        capability.temperature,
        30.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CurrentStateSurvivesNotificationConsumption)
{
    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle_,
            30.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        eventCapability{};

    ASSERT_EQ(
        RIManager_GetEnvironment(
            handle_,
            &eventCapability),
        RI_SUCCESS);

    rim::EnvironmentCapability
        currentCapability{};

    ASSERT_EQ(
        RIManager_GetCurrentEnvironment(
            handle_,
            &currentCapability),
        RI_SUCCESS);

    EXPECT_DOUBLE_EQ(
        currentCapability.temperature,
        30.0);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    CurrentStateCanBeReadMultipleTimes)
{
    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle_,
            35.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        first{};

    rim::EnvironmentCapability
        second{};

    ASSERT_EQ(
        RIManager_GetCurrentEnvironment(
            handle_,
            &first),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_GetCurrentEnvironment(
            handle_,
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
    NotificationIsConsumed)
{
    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle_,
            40.0),
        RI_SUCCESS);

    WaitPipeline();

    rim::EnvironmentCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetEnvironment(
            handle_,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        RIManager_GetEnvironment(
            handle_,
            &capability),
        RI_NO_DATA);
}






TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentError)
{
    ASSERT_EQ(
        RIManager_AddError(
            handle_,
            1001),
        RI_SUCCESS);

    WaitPipeline();

    rim::ErrorCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetCurrentError(
            handle_,
            &capability),
        RI_SUCCESS);

    ASSERT_EQ(
        capability.errors.size(),
        1U);

    EXPECT_EQ(
        capability.errors.front().errorCode,
        1001U);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    ErrorStateSurvivesNotificationConsumption)
{
    ASSERT_EQ(
        RIManager_AddError(
            handle_,
            1001),
        RI_SUCCESS);

    WaitPipeline();

    rim::ErrorCapability
        eventCapability{};

    ASSERT_EQ(
        RIManager_GetError(
            handle_,
            &eventCapability),
        RI_SUCCESS);

    rim::ErrorCapability
        currentCapability{};

    ASSERT_EQ(
        RIManager_GetCurrentError(
            handle_,
            &currentCapability),
        RI_SUCCESS);

    ASSERT_EQ(
        currentCapability.errors.size(),
        1U);

    EXPECT_EQ(
        currentCapability.errors.front().errorCode,
        1001U);
}



TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentPrintReady)
{
    ASSERT_EQ(
        RIManager_TestInjectUpperDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectRightDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectLeftDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    WaitPipeline();

    rim::PrintReadyCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetCurrentPrintReady(
            handle_,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    PrintReadyStateSurvivesNotificationConsumption)
{
    ASSERT_EQ(
        RIManager_TestInjectUpperDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectRightDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectLeftDoorOpen(
            handle_,
            0),
        RI_SUCCESS);

    WaitPipeline();

    rim::PrintReadyCapability
        eventCapability{};

    ASSERT_EQ(
        RIManager_GetPrintReady(
            handle_,
            &eventCapability),
        RI_SUCCESS);

    rim::PrintReadyCapability
        currentCapability{};

    ASSERT_EQ(
        RIManager_GetCurrentPrintReady(
            handle_,
            &currentCapability),
        RI_SUCCESS);

    EXPECT_TRUE(
        currentCapability.ready);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentConsumable)
{
    ASSERT_EQ(
        RIManager_TestInjectStapleLevel(
            handle_,
            80),
        RI_SUCCESS);

    WaitPipeline();

    rim::ConsumableCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetCurrentConsumable(
            handle_,
            &capability),
        RI_SUCCESS);

    EXPECT_EQ(
        capability.stapleLevel,
        80);
}

TEST_F(
    EndToEndCApiCurrentStateTest,
    GetCurrentJob)
{
    ASSERT_EQ(
        RIManager_TestInjectJobActive(
            handle_,
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectJobId(
            handle_,
            123),
        RI_SUCCESS);

    WaitPipeline();

    rim::JobCapability
        capability{};

    ASSERT_EQ(
        RIManager_GetCurrentJob(
            handle_,
            &capability),
        RI_SUCCESS);

    EXPECT_TRUE(
        capability.jobActive);

    EXPECT_EQ(
        capability.jobId,
        123);
}

