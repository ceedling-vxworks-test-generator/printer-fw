#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

#include "MachineCapabilityStore.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTrigger.hpp"
#include "NotificationTargetType.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

TEST(
    EndToEndNotificationFlowTest,
    EnvironmentNotification)
{
    rim::MachineCapabilityStore store;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        1
    };

    info.method =
        rim::DeliveryMethod::Mailbox;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    store.Store(
        input);

    publishManager.Publish(
        info.target,
        rim::NotificationTrigger::OnChange);

    rim::NotificationReceiver
        receiver(
            mailboxManager);

    rim::NotificationMessage
        message{};

    ASSERT_TRUE(
        receiver.TryGetNotification(
            subscriptionId,
            message));

    EXPECT_EQ(
        message.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        message.target.id,
        1U);

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    EndToEndNotificationFlowTest,
    PrintReadyNotification)
{
    rim::MachineCapabilityStore store;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        2
    };

    info.method =
        rim::DeliveryMethod::Mailbox;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::PrintReadyCapability input{};

    input.ready = true;

    store.Store(
        input);

    publishManager.Publish(
        info.target,
        rim::NotificationTrigger::OnChange);

    rim::NotificationReceiver
        receiver(
            mailboxManager);

    rim::NotificationMessage
        message{};

    ASSERT_TRUE(
        receiver.TryGetNotification(
            subscriptionId,
            message));

    EXPECT_EQ(
        message.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        message.target.id,
        2U);

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}