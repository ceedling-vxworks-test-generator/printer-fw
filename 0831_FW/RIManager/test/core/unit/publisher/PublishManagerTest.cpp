#include <gtest/gtest.h>
#include <chrono>

#include "rim_api.h"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

#include "PeriodicNotifyManager.hpp"
#include "Product.hpp"
#include "test/core/support/NotificationTestHelper.hpp"


class PublishManagerTest
    : public ::testing::Test
{
protected:

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue
        callbackQueue;

    rim::ChangeNotifyManager
        notifyManager
        {
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue
        };

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::ProductContext
        productContext
        {
            rim::kProductDefinition
        };

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };
};

TEST_F(
    PublishManagerTest,
    ProcessPeriodicNotifications)
{

    subscriptionStore.Register(
    {
        100,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::Periodic
    });

    rim::PeriodicCondition
        condition{};

    condition.subscriptionId = 100;

    condition.interval =
        std::chrono::seconds(
            10);

    condition.nextNotifyTime =
        std::chrono::steady_clock::now()
        - std::chrono::seconds(
            1);

    periodicNotifyManager.Register(
        condition);

    publishManager.
        ProcessPeriodicNotifications();

    std::vector<
        rim::PeriodicCondition>
        due;

    periodicNotifyManager.
        GetDueConditions(
            due);

    EXPECT_TRUE(
        due.empty());
}
TEST_F(
    PublishManagerTest,
    PublishOnChange)
{
    subscriptionStore.Register(
    {
        100,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Mailbox,
        rim::NotificationTrigger::OnChange
    });

    publishManager.Publish(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        1u,
        mailboxManager.GetMailbox(
            100).Count());
}

TEST_F(
    PublishManagerTest,
    PublishWithoutSubscriber)
{
    EXPECT_NO_THROW(
        publishManager.Publish(
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::NotificationTrigger::OnChange));
}

TEST_F(
    PublishManagerTest,
    PeriodicNotificationDeliveredToMailbox)
{
    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    subscriptionStore.Register(
    {
        subscriptionId,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Mailbox,
        rim::NotificationTrigger::Periodic
    });

    rim::PeriodicCondition
        condition{};

    condition.subscriptionId =
        subscriptionId;

    condition.interval =
        std::chrono::seconds(10);

    condition.nextNotifyTime =
        std::chrono::steady_clock::now()
        - std::chrono::seconds(1);

    periodicNotifyManager.Register(
        condition);

    publishManager.
        ProcessPeriodicNotifications();

    rim::NotificationMessage
        message{};

    ASSERT_TRUE(
        mailboxManager
            .GetMailbox(
                subscriptionId)
            .Pop(
                message));

    EXPECT_EQ(
        message.target.id,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::Periodic);
}

TEST_F(
    PublishManagerTest,
    PublishUnknownTargetDoesNotThrow)
{
    EXPECT_NO_THROW(
        publishManager.Publish(
            test::CapabilityTarget(999999),
            rim::NotificationTrigger::OnChange));
}