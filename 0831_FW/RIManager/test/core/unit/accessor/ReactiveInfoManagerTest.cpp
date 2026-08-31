#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

#include "SubscriberMailboxManager.hpp"
#include "SubscriptionId.hpp"

#include "NotificationMessage.hpp"
#include "RIMId.hpp"
#include "NotificationTrigger.hpp"

TEST(
    ReactiveInfoManagerTest,
    TryGetNotification)
{
    rim::SubscriberMailboxManager
        mailboxManager;

    constexpr rim::SubscriptionId
        subscriptionId{1};

    rim::NotificationMessage
        input{};

    input.target.type =
        rim::RIMIdType::Capability;

    input.target.id = 1;

    input.trigger =
        rim::NotificationTrigger::OnChange;

    mailboxManager
        .GetMailbox(
            subscriptionId)
        .Push(
            input);

    rim::NotificationReceiver
        receiver(
            mailboxManager);

    rim::NotificationMessage
        output{};

    ASSERT_TRUE(
        receiver.TryGetNotification(
            subscriptionId,
            output));

    EXPECT_EQ(
        output.target.type,
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        output.target.id,
        1U);

    EXPECT_EQ(
        output.trigger,
        rim::NotificationTrigger::OnChange);
}
