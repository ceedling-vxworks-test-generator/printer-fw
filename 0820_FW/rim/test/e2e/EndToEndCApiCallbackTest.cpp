#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "rim_api.h"

namespace
{

std::atomic<std::uint64_t>
    g_receivedId{0U};

std::atomic<bool>
    g_environmentCalled{false};

void OnEnvironment(
    std::uint64_t subscriptionId,
    const void* notificationMessage,
    void* userData)
{
    (void)notificationMessage;
    (void)userData;

    g_receivedId = subscriptionId;
    g_environmentCalled = true;
}

std::atomic<bool>
    g_printReadyCalled{false};

void OnPrintReady(
    std::uint64_t subscriptionId,
    const void* notificationMessage,
    void* userData)
{
    (void)subscriptionId;
    (void)notificationMessage;
    (void)userData;

    g_printReadyCalled = true;
}

std::atomic<bool>
    g_consumableCalled{false};

void OnConsumable(
    std::uint64_t subscriptionId,
    const void* notificationMessage,
    void* userData)
{
    (void)subscriptionId;
    (void)notificationMessage;
    (void)userData;

    g_consumableCalled = true;
}

std::atomic<bool>
    g_jobCalled{false};

void OnJob(
    std::uint64_t subscriptionId,
    const void* notificationMessage,
    void* userData)
{
    (void)subscriptionId;
    (void)notificationMessage;
    (void)userData;

    g_jobCalled = true;
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeReceiveAndUnsubscribe)
{
    g_environmentCalled = false;

    g_receivedId = 0U;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    std::uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIM_TestInjectTemperature(
            30.0),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_environmentCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_environmentCalled);

    EXPECT_EQ(
        g_receivedId,
        id);

    ASSERT_EQ(
        RIM_Unsubscribe(
            id),
        RI_SUCCESS);

    g_environmentCalled = false;

    ASSERT_EQ(
        RIM_TestInjectTemperature(
            31.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    EXPECT_FALSE(
        g_environmentCalled);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribePrintReady)
{
    g_printReadyCalled = false;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_PRINT_READY,
            OnPrintReady,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

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

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_printReadyCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_printReadyCalled);

    ASSERT_EQ(
        RIM_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeConsumable)
{
    g_consumableCalled = false;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_CONSUMABLE,
            OnConsumable,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIM_TestInjectStapleLevel(
            80),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_consumableCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_consumableCalled);

    ASSERT_EQ(
        RIM_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeJob)
{
    g_jobCalled = false;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_JOB,
            OnJob,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIM_TestInjectJobActive(
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_TestInjectJobId(
            123),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_jobCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_jobCalled);

    ASSERT_EQ(
        RIM_Unsubscribe(
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

} // namespace