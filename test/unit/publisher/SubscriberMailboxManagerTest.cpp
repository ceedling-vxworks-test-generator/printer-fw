#include <gtest/gtest.h>

#include "SubscriberMailboxManager.hpp"

namespace rim
{
namespace
{

TEST(
    SubscriberMailboxManagerTest,
    ReturnSameMailboxForSameId)
{
    SubscriberMailboxManager manager;

    auto& mailbox1 =
        manager.GetMailbox(1);

    auto& mailbox2 =
        manager.GetMailbox(1);

    EXPECT_EQ(
        &mailbox1,
        &mailbox2);
}

TEST(
    SubscriberMailboxManagerTest,
    ReturnDifferentMailboxForDifferentId)
{
    SubscriberMailboxManager manager;

    auto& mailbox1 =
        manager.GetMailbox(1);

    auto& mailbox2 =
        manager.GetMailbox(2);

    EXPECT_NE(
        &mailbox1,
        &mailbox2);
}

TEST(
    SubscriberMailboxManagerTest,
    GetOverflowCountUnknownMailbox)
{
    rim::SubscriberMailboxManager manager;

    EXPECT_EQ(
        manager.GetOverflowCount(999),
        0U);
}

TEST(
    SubscriberMailboxManagerTest,
    GetOverflowCount)
{
    rim::SubscriberMailboxManager manager;

    auto& mailbox =
        manager.GetMailbox(100);

    rim::NotificationMessage msg{};

    for (int i = 0; i < 1001; ++i)
    {
        mailbox.Push(msg);
    }

    EXPECT_EQ(
        manager.GetOverflowCount(100),
        1U);
}

} // namespace
} // namespace rim