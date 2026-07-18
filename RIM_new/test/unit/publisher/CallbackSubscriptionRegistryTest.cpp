#include <gtest/gtest.h>

#include "publisher/CallbackSubscriptionRegistry.hpp"

TEST(
    CallbackSubscriptionRegistryTest,
    InvokeCallback)
{
    rim::CallbackSubscriptionRegistry
        registry;

    bool called = false;

    registry.SubscribeEnvironment(
        [&](const rim::EnvironmentCapability& capability)
        {
            (void)capability;

            called = true;
        });

    rim::EnvironmentCapability cap{};

    registry.NotifyEnvironment(
        cap);

    EXPECT_TRUE(
        called);
}