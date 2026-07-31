#include <gtest/gtest.h>

#include "ChangeNotifyManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

TEST(
    ChangeNotifyManagerTest,
    NotifySubscribedClient)
{
    rim::SubscriptionRegistry registry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager manager(
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

// TEST(
//     ChangeNotifyManagerTest,
//     IgnoreNonSubscribedClient)
// {
//     rim::SubscriptionRegistry registry;

//     rim::SubscriberMailbox mailbox;

//     rim::CallbackSubscriptionRegistry
//         callbackRegistry;

//     rim::ChangeNotifyManager manager(
//         mailbox,
//         callbackRegistry);

//     rim::EnvironmentCapability input{};

//     manager.Notify(
//         input);

//     rim::EnvironmentCapability output{};

//     EXPECT_FALSE(
//         mailbox.Pop(
//             output));
// }

TEST(
    ChangeNotifyManagerTest,
    NotifyEnvironmentCallback)
{
    rim::SubscriptionRegistry
        registry;

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.SubscribeEnvironment(
        [&](rim::SubscriptionId subscriptionId,
            const rim::EnvironmentCapability& capability)
        {
            (void)subscriptionId;
            (void)capability;

            called = true;
        });

    rim::ChangeNotifyManager manager(
            mailbox,
            callbackRegistry);

    rim::EnvironmentCapability
        capability{};

    manager.Notify(
        capability);

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    Unsubscribe)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    const auto id =
        registry.SubscribeEnvironment(
            [&](rim::SubscriptionId,
                const rim::EnvironmentCapability)          {
                called = true;
            });

    EXPECT_TRUE(
        registry.Unsubscribe(
            id));

    registry.NotifyEnvironment(
        rim::EnvironmentCapability{});

    EXPECT_FALSE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyErrorCallback)
{
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribeError(
        [&](rim::SubscriptionId,
            const rim::ErrorCapability&)
        {
            called = true;
        });

    rim::ChangeNotifyManager manager(
        mailbox,
        registry);

    manager.Notify(
        rim::ErrorCapability{});

    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyPrintReadyCallback)
{
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribePrintReady(
        [&](rim::SubscriptionId,
            const rim::PrintReadyCapability&)
        {
            called = true;
        });

    rim::ChangeNotifyManager manager(
        mailbox,
        registry);

    manager.Notify(
        rim::PrintReadyCapability{});

    EXPECT_TRUE(
        called);
}