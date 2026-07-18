#include <gtest/gtest.h>

#include "capability/MachineCapabilityStore.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriberMailbox.hpp"
#include "publisher/SubscriptionRegistry.hpp"

TEST(
    CapabilityToPublisherTest,
    NoNotifyWhenSameValue)
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

    ASSERT_FALSE(
        publishManager.HasEnvironmentChanged());
}