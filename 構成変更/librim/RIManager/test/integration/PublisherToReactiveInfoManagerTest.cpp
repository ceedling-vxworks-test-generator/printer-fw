#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

#include "MachineCapabilityStore.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"
#include "SubscriptionRegistry.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

TEST(
    PublisherToReactiveInfoManagerTest,
    ReceiveEnvironmentNotification)
{
    rim::MachineCapabilityStore store;

    rim::SubscriptionRegistry
        subscriptionRegistry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    publisherRegistry.Register(
        "Environment",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    store.GetEnvironment());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::EnvironmentCapability capability{};

    capability.temperature = 300.15;
    capability.humidity = 60.0;

    store.Store(
        capability);

    publishManager.Publish(
        "Environment");

    rim::NotificationReceiver
        receiver(
            mailbox);

    rim::EnvironmentCapability
        received{};

    ASSERT_TRUE(
        receiver.TryGetEnvironment(
            received));

    EXPECT_DOUBLE_EQ(
        received.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        received.humidity,
        60.0);
}