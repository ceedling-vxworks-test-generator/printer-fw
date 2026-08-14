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

} // namespace
} // namespace rim