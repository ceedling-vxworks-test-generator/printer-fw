#include <gtest/gtest.h>

#include "SubscriberMailbox.hpp"

TEST(
    MailboxTest,
    PushAndPop)
{
    rim::SubscriberMailbox mailbox;

    rim::EnvironmentCapability cap{};

    cap.temperature = 300.15;
    cap.humidity = 60.0;

    mailbox.Push(cap);

    rim::EnvironmentCapability out{};

    ASSERT_TRUE(
        mailbox.Pop(
            out));

    EXPECT_DOUBLE_EQ(
        out.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        out.humidity,
        60.0);
}