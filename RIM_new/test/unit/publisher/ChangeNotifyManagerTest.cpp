#include <gtest/gtest.h>

#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/CallbackSubscriptionRegistry.hpp"

TEST(
    ChangeNotifyManagerTest,
    NotifySubscribedClient)
{
    rim::SubscriptionRegistry registry;

    registry.Subscribe(
        1);

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager manager(
        registry,
        mailbox,
        callbackRegistry);

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    manager.Notify(
        input);

    rim::EnvironmentCapability output{};

    ASSERT_TRUE(
        mailbox.Pop(
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);
}

TEST(
    ChangeNotifyManagerTest,
    IgnoreNonSubscribedClient)
{
    rim::SubscriptionRegistry registry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager manager(
        registry,
        mailbox,
        callbackRegistry);

    rim::EnvironmentCapability input{};

    manager.Notify(
        input);

    rim::EnvironmentCapability output{};

    EXPECT_FALSE(
        mailbox.Pop(
            output));
}

TEST(
    ChangeNotifyManagerTest,
    NotifyCallback)
{
    rim::SubscriptionRegistry
        registry;

    registry.Subscribe(
        1);

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.
        SubscribeEnvironment(
            [&](const rim::EnvironmentCapability&)
            {
                called = true;
            });

    rim::ChangeNotifyManager
        manager(
            registry,
            mailbox,
            callbackRegistry);

    rim::EnvironmentCapability
        capability{};

    manager.Notify(
        capability);

    EXPECT_TRUE(
        called);
}