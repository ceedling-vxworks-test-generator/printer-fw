#include <gtest/gtest.h>

#include "rim_api.h"

#include "SubscriberMailbox.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTrigger.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    MailboxTest,
    PushAndPop)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage message{};

    ASSERT_TRUE(
        mailbox.Pop(
            message));

    EXPECT_EQ(
    message.target.type,
    rim::RIMIdType::Capability);

    EXPECT_EQ(
    message.target.id,
    static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    MailboxTest,
    PopEmptyMailbox)
{
    rim::SubscriberMailbox mailbox;

    rim::NotificationMessage message{};

    EXPECT_FALSE(
        mailbox.Pop(
            message));
}

TEST(
    MailboxTest,
    PreserveOrder)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange
    });

    mailbox.Push(
    {
        test::CapabilityTarget(RI_CAPABILITY_ERROR),
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage first{};
    rim::NotificationMessage second{};

    ASSERT_TRUE(
        mailbox.Pop(
            first));

    ASSERT_TRUE(
        mailbox.Pop(
            second));

    EXPECT_EQ(
        first.target.type,
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        first.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        second.target.type,
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        second.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_ERROR));

    EXPECT_EQ(
        first.trigger,
        rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        second.trigger,
        rim::NotificationTrigger::OnChange);
}