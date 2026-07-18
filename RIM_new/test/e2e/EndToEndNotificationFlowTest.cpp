#include <gtest/gtest.h>

#include "accessor/NotificationReceiver.hpp"

#include "capability/MachineCapabilityStore.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriberMailbox.hpp"
#include "publisher/SubscriptionRegistry.hpp"
#include "publisher/CallbackSubscriptionRegistry.hpp"

TEST(
    EndToEndNotificationFlowTest,
    EnvironmentNotification)
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

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    store.Store(
        input);

    ASSERT_TRUE(
        publishManager.HasEnvironmentChanged());

    rim::NotificationReceiver manager(
        mailbox);

    rim::EnvironmentCapability output{};

    ASSERT_TRUE(
        manager.TryGetEnvironment(
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output.humidity,
        60.0);
}

TEST(
    EndToEndNotificationFlowTest,
    ErrorNotification)
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

    rim::ErrorCapability input{};

    input.upperDoorOpen = true;

    store.Store(
        input);

    ASSERT_TRUE(
        publishManager.HasErrorChanged());

    rim::NotificationReceiver manager(
        mailbox);

    rim::ErrorCapability output{};

    ASSERT_TRUE(
        manager.TryGetError(
            output));

    EXPECT_TRUE(
        output.upperDoorOpen);
}

TEST(
    EndToEndNotificationFlowTest,
    PrintReadyNotification)
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

    rim::PrintReadyCapability input{};

    input.ready = true;

    store.Store(
        input);

    ASSERT_TRUE(
        publishManager.HasPrintReadyChanged());

    rim::NotificationReceiver manager(
        mailbox);

    rim::PrintReadyCapability output{};

    ASSERT_TRUE(
        manager.TryGetPrintReady(
            output));

    EXPECT_TRUE(
        output.ready);
}