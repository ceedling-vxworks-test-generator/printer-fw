#include <gtest/gtest.h>

#include "rim_api.h"

#include "SubscriberMailbox.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTrigger.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    SubscriberMailboxTest,
    PushPopPrintReadyNotification)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_EQ(
        output.target.type,
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        output.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_PRINT_READY));


    EXPECT_EQ(
        output.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    SubscriberMailboxTest,
    OverflowDetected)
{
    rim::SubscriberMailbox
        mailbox(1);

    const rim::NotificationMessage
        message{
            test::OnChangeMessage(RI_CAPABILITY_ENVIRONMENT)
        };

    EXPECT_TRUE(
        mailbox.Push(
            message));

    EXPECT_FALSE(
        mailbox.Push(
            message));

    EXPECT_EQ(
        mailbox.GetOverflowCount(),
        1U);
}