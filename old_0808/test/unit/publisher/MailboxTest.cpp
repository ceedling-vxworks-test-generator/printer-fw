#include <gtest/gtest.h>

#include "SubscriberMailbox.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTrigger.hpp"

TEST(
    MailboxTest,
    PushAndPop)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT)
        },
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage message{};

    ASSERT_TRUE(
        mailbox.Pop(
            message));

    EXPECT_EQ(
    message.target.type,
    rim::NotificationTargetType::Capability);

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
        {
            rim::NotificationTargetType::Capability,
            static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT)
        },
        rim::NotificationTrigger::OnChange
    });

    mailbox.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<uint32_t>(RI_CAPABILITY_ERROR)
        },
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
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        first.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        second.target.type,
        rim::NotificationTargetType::Capability);

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