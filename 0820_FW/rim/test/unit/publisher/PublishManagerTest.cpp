#include <gtest/gtest.h>
#include <chrono>

#include "ErrorInfo.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

#include "PeriodicNotifyManager.hpp"
#include "NotificationTargetType.hpp"

TEST(
    PublishManagerTest,
    ProcessPeriodicNotifications)
{
    rim::SubscriberMailbox
        mailbox;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore);

    subscriptionStore.Register(
    {
        100,
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
        },
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