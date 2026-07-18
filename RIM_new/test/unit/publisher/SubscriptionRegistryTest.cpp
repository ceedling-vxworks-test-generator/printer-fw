#include <gtest/gtest.h>

#include "publisher/SubscriptionRegistry.hpp"

TEST(
    SubscriptionRegistryTest,
    Subscribe)
{
    rim::SubscriptionRegistry registry;

    EXPECT_TRUE(
        registry.Subscribe(
            1));

    EXPECT_TRUE(
        registry.IsSubscribed(
            1));
}

TEST(
    SubscriptionRegistryTest,
    Unsubscribe)
{
    rim::SubscriptionRegistry registry;

    registry.Subscribe(
        1);

    EXPECT_TRUE(
        registry.Unsubscribe(
            1));

    EXPECT_FALSE(
        registry.IsSubscribed(
            1));
}
