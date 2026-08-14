#include <gtest/gtest.h>

#include "SubscriberMailbox.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTrigger.hpp"
#include "NotificationTargetType.hpp"

TEST(
    SubscriberMailboxTest,
    PushPopPrintReadyNotification)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<uint32_t>(RI_CAPABILITY_PRINT_READY)
        },
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_EQ(
        output.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        output.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_PRINT_READY));


    EXPECT_EQ(
        output.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    SubscriberMailboxTest,
    PushPopEnvironmentNotification)
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

    rim::NotificationMessage output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_EQ(
        output.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        output.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_ENVIRONMENT));


    EXPECT_EQ(
        output.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    SubscriberMailboxTest,
    PushPopErrorNotification)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<uint32_t>(RI_CAPABILITY_ERROR)
        },
        rim::NotificationTrigger::OnChange
    });

    rim::NotificationMessage output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_EQ(
        output.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        output.target.id,
        static_cast<uint32_t>(RI_CAPABILITY_ERROR));

    EXPECT_EQ(
        output.trigger,
        rim::NotificationTrigger::OnChange);
}