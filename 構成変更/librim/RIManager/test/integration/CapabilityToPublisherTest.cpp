#include <gtest/gtest.h>

#include "MachineCapabilityStore.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

#include "SubscriptionRegistry.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

TEST(
    CapabilityToPublisherTest,
    NotifyEnvironmentChanged)
{
    rim::MachineCapabilityStore store;

    rim::SubscriberMailbox mailbox;

    rim::SubscriptionRegistry
        subscriptionRegistry;

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

    rim::EnvironmentCapability out{};

    ASSERT_TRUE(
        mailbox.Pop(
            out));

    EXPECT_DOUBLE_EQ(
        out.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        out.humidity,
        60.0);
}