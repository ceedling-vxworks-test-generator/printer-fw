#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "rim_api.h"

namespace
{

std::atomic<bool> g_errorListCallbackCalled{
    false};

void OnErrorList(
    uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    g_errorListCallbackCalled = true;
}

TEST(
    EndToEndCApiErrorListNotificationTest,
    CallbackAndMailboxReceiveNotification)
{
    g_errorListCallbackCalled = false;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    uint64_t callbackId{};

    RI_SUBSCRIPTION_REQUEST callbackRequest{};

    callbackRequest.targetType =
        RI_TARGET_DATA;

    callbackRequest.targetId =
        RI_DATA_ERROR_LIST;

    callbackRequest.trigger =
        RI_TRIGGER_ON_CHANGE;

    callbackRequest.method =
        RI_METHOD_CALLBACK;

    callbackRequest.callback =
        OnErrorList;

    ASSERT_EQ(
        RIM_Subscribe(
            &callbackRequest,
            &callbackId),
        RI_SUCCESS);

    ASSERT_NE(
        callbackId,
        0U);

    uint64_t mailboxId{};

    RI_SUBSCRIPTION_REQUEST mailboxRequest{};

    mailboxRequest.targetType =
        RI_TARGET_DATA;

    mailboxRequest.targetId =
        RI_DATA_ERROR_LIST;

    mailboxRequest.trigger =
        RI_TRIGGER_ON_CHANGE;

    mailboxRequest.method =
        RI_METHOD_MAILBOX;

    ASSERT_EQ(
        RIM_Subscribe(
            &mailboxRequest,
            &mailboxId),
        RI_SUCCESS);

    ASSERT_NE(
        mailboxId,
        0U);

    RI_FAULT_INFO_LIST list{};

    list.count = 2;

    list.entries[0].code = 100;
    list.entries[0].state = 1;

    list.entries[1].code = 200;
    list.entries[1].state = 2;

    ASSERT_EQ(
        SetErrorList(
            &list),
        RI_SUCCESS);

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            1);

    while (!g_errorListCallbackCalled &&
           std::chrono::steady_clock::now()
                < timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    EXPECT_TRUE(
        g_errorListCallbackCalled);

    EXPECT_GT(
        RIM_GetMailboxCount(
            mailboxId),
        0U);

    rim_notification_message_t
        message{};

    ASSERT_EQ(
        RIM_GetNotification(
            mailboxId,
            &message),
        RI_SUCCESS);

    EXPECT_EQ(
        message.targetType,
        RI_TARGET_DATA);

    EXPECT_EQ(
        message.targetId,
        static_cast<uint32_t>(
            RI_DATA_ERROR_LIST));

    EXPECT_EQ(
        message.trigger,
        RI_TRIGGER_ON_CHANGE);

    RI_FAULT_INFO_LIST
        readBack{};

    ASSERT_EQ(
        GetErrorList(
            &readBack),
        RI_SUCCESS);

    ASSERT_EQ(
        readBack.count,
        2U);

    EXPECT_EQ(
        readBack.entries[0].code,
        100U);

    EXPECT_EQ(
        readBack.entries[0].state,
        1U);

    EXPECT_EQ(
        readBack.entries[1].code,
        200U);

    EXPECT_EQ(
        readBack.entries[1].state,
        2U);

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}

} // namespace