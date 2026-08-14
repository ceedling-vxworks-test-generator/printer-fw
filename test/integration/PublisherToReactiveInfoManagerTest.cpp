#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

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

#include "rim_capability_id.h"

TEST(
    PublisherToReactiveInfoManagerTest,
    ReceiveEnvironmentNotification)
{
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
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Mailbox;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

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
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}