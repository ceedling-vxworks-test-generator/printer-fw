#include <gtest/gtest.h>
#include <chrono>

#include "printer_a.h"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

#include "PeriodicNotifyManager.hpp"
#include "NotificationTargetType.hpp"
#include "PrinterAProductDefinition.hpp"

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

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition);

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
TEST(
    PublishManagerTest,
    PublishOnChange)
{
    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition);

    subscriptionStore.Register(
    {
        100,
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        },
        rim::DeliveryMethod::Mailbox,
        rim::NotificationTrigger::OnChange
    });

    publishManager.Publish(
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        },
        rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        1u,
        mailboxManager.GetMailbox(
            100).Count());
}

TEST(
    PublishManagerTest,
    PublishWithoutSubscriber)
{
    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition);

    EXPECT_NO_THROW(
        publishManager.Publish(
            {
                rim::NotificationTargetType::Capability,
                static_cast<std::uint32_t>(
                    RI_CAPABILITY_ENVIRONMENT)
            },
            rim::NotificationTrigger::OnChange));
}

TEST(
    PublishManagerTest,
    PeriodicNotificationDeliveredToMailbox)
{
    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition);

    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    subscriptionStore.Register(
    {
        subscriptionId,
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        },
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