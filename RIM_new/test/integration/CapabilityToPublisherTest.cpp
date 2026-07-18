#include <gtest/gtest.h>

#include "capability/MachineCapabilityStore.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriberMailbox.hpp"

TEST(
    CapabilityToPublisherTest,
    NotifyEnvironmentChanged)
{
    rim::MachineCapabilityStore store;

    rim::SubscriberMailbox mailbox;

    rim::SubscriptionRegistry registry;

    registry.Subscribe(
        1);

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