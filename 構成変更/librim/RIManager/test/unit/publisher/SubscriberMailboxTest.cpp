#include <gtest/gtest.h>

#include "SubscriberMailbox.hpp"

#include "PrintReadyCapability.hpp"

TEST(
    SubscriberMailboxTest,
    PushPopPrintReady)
{
    rim::SubscriberMailbox mailbox;

    rim::PrintReadyCapability input{};

    input.ready = true;

    mailbox.Push(
        input);

    rim::PrintReadyCapability output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_TRUE(
        output.ready);
}