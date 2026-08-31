#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <chrono>

#include "rim_api.h"

namespace
{

std::atomic<bool> g_received{false};

void DataNotificationCallback(
    uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    g_received.store(true);
}

}

TEST(
    EndToEndDataNotificationTest,
    ReceiveSpecifiedDataId)
{
    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    g_received.store(
        false);

    uint64_t subscriptionId{};

    RI_SUBSCRIPTION_REQUEST request{};

    request.targetType =
        RI_TARGET_DATA;
        // RI_TARGET_CAPABILITY;

    request.targetId =
        static_cast<uint32_t>(
            RI_DATA_UPPER_DOOR_OPEN);
            // RI_CAPABILITY_ENVIRONMENT);

    request.trigger =
        RI_TRIGGER_ON_CHANGE;

    request.method =
        RI_METHOD_CALLBACK;

    request.intervalMs =
        0;

    request.callback =
        DataNotificationCallback;

    ASSERT_EQ(
        RIM_Subscribe(
            &request,
            &subscriptionId),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetBool(
            RI_DATA_UPPER_DOOR_OPEN,
            1),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            100));

    EXPECT_TRUE(
        g_received.load());

    EXPECT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    EXPECT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

