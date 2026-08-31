#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "rim_api.h"

namespace
{

class EndToEndCApiCallbackTest
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
        EXPECT_EQ(
            RIM_Stop(),
            RI_SUCCESS);

        EXPECT_EQ(
            RIM_Destroy(),
            RI_SUCCESS);
    }
};

std::atomic<std::uint64_t>
    g_receivedId{0U};

std::atomic<bool>
    g_environmentCalled{false};

void OnEnvironment(
    std::uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)notificationMessage;

    g_receivedId = subscriptionId;
    g_environmentCalled = true;
}

std::atomic<bool>
    g_printReadyCalled{false};

void OnPrintReady(
    std::uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;
    g_printReadyCalled = true;
}

std::atomic<int>
    g_environmentCount{0};

void OnEnvironmentCount(
    std::uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    ++g_environmentCount;
}


std::atomic<int>
    g_printReadyCount{0};

void OnPrintReadyCount(
    std::uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    ++g_printReadyCount;
}

TEST_F(
    EndToEndCApiCallbackTest,
    SubscribeReceiveNotification)
{
    g_environmentCalled = false;

    g_receivedId = 0U;

    std::uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
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

    g_environmentCalled = false;

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            31.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    EXPECT_FALSE(
        g_environmentCalled);
}

TEST_F(
    EndToEndCApiCallbackTest,
    SubscribePrintReady)
{
    g_printReadyCalled = false;

    uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_PRINT_READY,
            OnPrintReady,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

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
}

TEST_F(
    EndToEndCApiCallbackTest,
    SameEnvironmentValueDoesNotNotifyAgain)
{
    g_environmentCount = 0;

    std::uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironmentCount,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    //
    // 初回通知
    //
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    const int firstCount =
        g_environmentCount.load();

    EXPECT_GE(
        firstCount,
        1);

    //
    // 同値更新
    //
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            30.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    EXPECT_EQ(
        firstCount,
        g_environmentCount.load());
}

TEST_F(
    EndToEndCApiCallbackTest,
    PrintReadyChangedNotifiesAgain)
{
    g_printReadyCount = 0;

    std::uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_PRINT_READY,
            OnPrintReadyCount,
            &id),
        RI_SUCCESS);

    ASSERT_NE(
        id,
        0U);

    //
    // 初期状態
    // 全ドア閉
    //
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

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    const auto firstCount =
        g_printReadyCount.load();

    EXPECT_GE(
        firstCount,
        1);

    //
    // 上扉を開く
    //
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            true),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    EXPECT_GT(
        g_printReadyCount.load(),
        firstCount);
}

TEST_F(
    EndToEndCApiCallbackTest,
    SamePrintReadyValueDoesNotNotifyAgain)
{
    g_printReadyCount = 0;

    std::uint64_t id{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_PRINT_READY,
            OnPrintReadyCount,
            &id),
        RI_SUCCESS);

    //
    // 初回
    //
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    const auto firstCount =
        g_printReadyCount.load();

    EXPECT_GE(
        firstCount,
        1);

    //
    // 同値
    //
    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            false),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            200));

    EXPECT_EQ(
        firstCount,
        g_printReadyCount.load());
}

} // namespace