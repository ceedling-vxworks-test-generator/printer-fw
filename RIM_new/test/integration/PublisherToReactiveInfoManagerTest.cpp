#include <gtest/gtest.h>

#include "accessor/NotificationReceiver.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriberMailbox.hpp"
#include "publisher/SubscriptionRegistry.hpp"

TEST(
    PublisherToReactiveInfoManagerTest,
    ReceiveEnvironmentNotification)
{
    rim::MachineCapabilityStore store;

    rim::SubscriptionRegistry registry;

    registry.Subscribe(
        1);

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        registry,
        mailbox,
        callbackRegistry);

    rim::PublishManager publishManager(
        store,
        notifyManager);

    rim::EnvironmentCapability capability{};

    capability.temperature = 300.15;
    capability.humidity = 60.0;

    store.Store(
        capability);

    ASSERT_TRUE(
        publishManager.HasEnvironmentChanged());

    rim::NotificationReceiver manager(
        mailbox);

    rim::EnvironmentCapability received{};

    ASSERT_TRUE(
        manager.TryGetEnvironment(
            received));

    EXPECT_DOUBLE_EQ(
        received.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        received.humidity,
        60.0);
}