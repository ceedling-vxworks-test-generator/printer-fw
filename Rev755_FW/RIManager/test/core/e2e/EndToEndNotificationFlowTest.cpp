#include <gtest/gtest.h>

#include "rim_api.h"

#include "NotificationReceiver.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "NotificationMessage.hpp"
#include "RIMId.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"
#include "Product.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

class EndToEndNotificationFlowTest
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
    EndToEndNotificationFlowTest,
    EnvironmentNotification)
{
    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = subscriptionId;

    info.target =
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

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
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        message.target.id,
        RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST_F(
    EndToEndNotificationFlowTest,
    PrintReadyNotification)
{
    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = subscriptionId;

    info.target =
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY);

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
        rim::RIMIdType::Capability);

    EXPECT_EQ(
        message.target.id,
        RI_CAPABILITY_PRINT_READY);

    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}