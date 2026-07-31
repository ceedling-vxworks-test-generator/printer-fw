#include <gtest/gtest.h>

#include "SubscriptionRegistry.hpp"

TEST(
    SubscriptionRegistryTest,
    Subscribe)
{
    rim::SubscriptionRegistry registry;

    const auto id =
        registry.Subscribe();

    EXPECT_TRUE(
        registry.IsSubscribed(
            id));
}

TEST(
    SubscriptionRegistryTest,
    Unsubscribe)
{
    rim::SubscriptionRegistry registry;

    const auto id =
        registry.Subscribe();

    EXPECT_TRUE(
        registry.Unsubscribe(
            id));
}
