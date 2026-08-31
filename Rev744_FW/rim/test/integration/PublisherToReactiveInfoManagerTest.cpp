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
#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

TEST(
    PublisherToReactiveInfoManagerTest,
    ReceiveEnvironmentNotification)
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

    const rim::SubscriptionId
        subscriptionId =
            subscriptionStore.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id =
        subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,

        // Environment Capability ID
        1U
    };

    info.method =
        rim::DeliveryMethod::Mailbox;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::EnvironmentCapability capability{};

    capability.temperature = 300.15;
    capability.humidity = 60.0;

    store.Store(
        capability);

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
        info.target.id);

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}