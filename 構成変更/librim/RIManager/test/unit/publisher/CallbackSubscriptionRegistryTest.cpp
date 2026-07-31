#include <gtest/gtest.h>

#include "CallbackSubscriptionRegistry.hpp"

TEST(
    CallbackSubscriptionRegistryTest,
    InvokeCallback)
{
    bool called = false;

    rim::SubscriptionId receivedId{};

    rim::CallbackSubscriptionRegistry callbackRegistry;

    const auto id =
        callbackRegistry.SubscribeEnvironment(
            [&](rim::SubscriptionId subscriptionId,
                const rim::EnvironmentCapability& capability)
            {
                (void)subscriptionId;
                (void)capability;

                called = true;
            });

    rim::EnvironmentCapability cap{};

    callbackRegistry.NotifyEnvironment(
        cap);

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    GenerateUniqueSubscriptionId)
{
    rim::CallbackSubscriptionRegistry
        registry;

    const auto id1 =
        registry.SubscribeEnvironment(
            [&](rim::SubscriptionId,
                const rim::EnvironmentCapability&)
            {
            });

    const auto id2 =
        registry.SubscribeEnvironment(
            [&](rim::SubscriptionId,
                const rim::EnvironmentCapability&)
            {
            });

    EXPECT_NE(
        id1,
        id2);
}

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeRemovesCallback)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    const auto id =
        registry.SubscribeEnvironment(
            [&](rim::SubscriptionId,
                const rim::EnvironmentCapability&)
            {
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
    CallbackSubscriptionRegistryTest,
    EnvironmentError)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool environmentCalled = false;

    bool errorCalled = false;

    registry.SubscribeEnvironment(
        [&](rim::SubscriptionId subscriptionId,
            const rim::EnvironmentCapability& capability)
        {
            (void)subscriptionId;
            (void)capability;

            environmentCalled = true;
        });

    registry.SubscribeError(
        [&](rim::SubscriptionId subscriptionId,
            const rim::ErrorCapability& capability)
        {
            (void)subscriptionId;
            (void)capability;

            errorCalled = true;
        });

    registry.NotifyEnvironment(
        rim::EnvironmentCapability{});

    EXPECT_TRUE(
        environmentCalled);

    EXPECT_FALSE(
        errorCalled);

    environmentCalled = false;
    errorCalled = false;

    registry.NotifyError(
        rim::ErrorCapability{});

    EXPECT_FALSE(
        environmentCalled);

    EXPECT_TRUE(
        errorCalled);
}

TEST(
    CallbackSubscriptionRegistryTest,
    NotifyError)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribeError(
        [&](rim::SubscriptionId,
            const rim::ErrorCapability&)
        {
            called = true;
        });

    registry.NotifyError(
        rim::ErrorCapability{});

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    NotifyPrintReady)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribePrintReady(
        [&](rim::SubscriptionId,
            const rim::PrintReadyCapability&)
        {
            called = true;
        });

    registry.NotifyPrintReady(
        rim::PrintReadyCapability{});

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    NotifyConsumable)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribeConsumable(
        [&](rim::SubscriptionId,
            const rim::ConsumableCapability&)
        {
            called = true;
        });

    registry.NotifyConsumable(
        rim::ConsumableCapability{});

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    NotifyJob)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribeJob(
        [&](rim::SubscriptionId,
            const rim::JobCapability&)
        {
            called = true;
        });

    rim::JobCapability capability{};

    capability.jobActive = true;
    capability.jobId = 123;

    registry.NotifyJob(
        capability);

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeJob)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    const auto id =
        registry.SubscribeJob(
            [&](rim::SubscriptionId,
                const rim::JobCapability&)
            {
                called = true;
            });

    EXPECT_TRUE(
        registry.Unsubscribe(
            id));

    rim::JobCapability capability{};

    capability.jobActive = true;

    registry.NotifyJob(
        capability);

    EXPECT_FALSE(
        called);
}
