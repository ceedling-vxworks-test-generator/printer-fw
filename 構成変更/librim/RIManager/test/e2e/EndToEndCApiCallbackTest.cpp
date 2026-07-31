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
    const void* capability,
    void* userData)
{
    (void)capability;
    (void)userData;

    g_receivedId =
        subscriptionId;

    g_environmentCalled = true;
}

std::atomic<bool>
    g_errorCalled{false};

void OnError(
    std::uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    g_errorCalled = true;
}

std::atomic<bool>
    g_printReadyCalled{false};

void OnPrintReady(
    std::uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    g_printReadyCalled = true;
}

std::atomic<bool>
    g_consumableCalled{false};

void OnConsumable(
    std::uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    g_consumableCalled = true;
}

std::atomic<bool>
    g_jobCalled{false};

void OnJob(
    std::uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    g_jobCalled = true;
}


TEST(
    EndToEndCApiCallbackTest,
    SubscribeReceiveAndUnsubscribe)
{
    g_environmentCalled = false;

    g_receivedId = 0U;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    std::uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeEnvironment(
            handle,
            OnEnvironment,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle,
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
        RIManager_Unsubscribe(
            handle,
            id),
        RI_SUCCESS);

    g_environmentCalled = false;

    ASSERT_EQ(
        RIManager_TestInjectTemperature(
            handle,
            31.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    EXPECT_FALSE(
        g_environmentCalled);

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeError)
{
    g_errorCalled = false;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeError(
            handle,
            OnError,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_AddError(
            handle,
            1001),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_errorCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_errorCalled);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            handle,
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribePrintReady)
{
    g_printReadyCalled = false;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribePrintReady(
            handle,
            OnPrintReady,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_TestInjectUpperDoorOpen(
            handle,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectRightDoorOpen(
            handle,
            0),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectLeftDoorOpen(
            handle,
            0),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

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
        RIManager_Unsubscribe(
            handle,
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeConsumable)
{
    g_consumableCalled = false;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeConsumable(
            handle,
            OnConsumable,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_TestInjectStapleLevel(
            handle,
            80),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

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
        RIManager_Unsubscribe(
            handle,
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}

TEST(
    EndToEndCApiCallbackTest,
    SubscribeJob)
{
    g_jobCalled = false;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    uint64_t id{};

    ASSERT_EQ(
        RIManager_SubscribeJob(
            handle,
            OnJob,
            nullptr,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIManager_TestInjectJobActive(
            handle,
            1),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_TestInjectJobId(
            handle,
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
        RIManager_Unsubscribe(
            handle,
            id),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}


}
