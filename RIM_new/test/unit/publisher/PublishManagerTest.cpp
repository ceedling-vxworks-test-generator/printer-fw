#include <gtest/gtest.h>

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriberMailbox.hpp"
#include "publisher/SubscriptionRegistry.hpp"

TEST(
    PublishManagerTest,
    DetectEnvironmentUpdate)
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

    rim::PublishManager manager(
        store,
        notifyManager);

    rim::EnvironmentCapability cap{};

    cap.temperature = 300.15;
    cap.humidity = 60.0;

    store.Store(
        cap);

    EXPECT_TRUE(
        manager.HasEnvironmentChanged());

    EXPECT_FALSE(
        manager.HasEnvironmentChanged());
}