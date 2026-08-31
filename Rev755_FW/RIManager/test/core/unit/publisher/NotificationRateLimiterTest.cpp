#include <gtest/gtest.h>

#include "rim_api.h"

#include "NotificationRateLimiter.hpp"

#include <thread>
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    NotificationRateLimiterTest,
    IgnoreRapidNotification)
{
    rim::NotificationRateLimiter
        limiter;

    rim::RIMId target = 
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    EXPECT_TRUE(
        limiter.ShouldNotify(
            target,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            target,100));
}

TEST(
    NotificationRateLimiterTest,
    DifferentTargetCanNotify)
{
    rim::NotificationRateLimiter limiter;

    rim::RIMId environment = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    rim::RIMId printReady = 
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY);

    EXPECT_TRUE(
        limiter.ShouldNotify(
            environment,100));

    EXPECT_TRUE(
        limiter.ShouldNotify(
            printReady,100));
}

TEST(
    NotificationRateLimiterTest,
    SameTargetBlocked)
{
    rim::NotificationRateLimiter limiter;

    rim::RIMId target = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    EXPECT_TRUE(
        limiter.ShouldNotify(
            target,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            target,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            target,100));
}

TEST(
    NotificationRateLimiterTest,
    DifferentCapabilityIndependent)
{
    rim::NotificationRateLimiter
        limiter;

    rim::RIMId environment = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    rim::RIMId consumable = 
            test::CapabilityTarget(RI_CAPABILITY_CONSUMABLE);

    EXPECT_TRUE(
        limiter.ShouldNotify(
            environment,100));

    EXPECT_TRUE(
        limiter.ShouldNotify(
            consumable,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            environment,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            consumable,100));
}

TEST(
    NotificationRateLimiterTest,
    CanNotifyAgainAfterInterval)
{
    rim::NotificationRateLimiter limiter;

    rim::RIMId target = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    EXPECT_TRUE(
        limiter.ShouldNotify(
            target,100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            target,100));

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            120));

    EXPECT_TRUE(
        limiter.ShouldNotify(
            target,100));
}

TEST(
    NotificationRateLimiterTest,
    DifferentTargetTypesWithSameIdAreIndependent)
{
    rim::NotificationRateLimiter
        limiter;

    rim::RIMId capability{
        rim::RIMIdType::Capability,
        1};

    rim::RIMId data{
        rim::RIMIdType::Data,
        1};

    EXPECT_TRUE(
        limiter.ShouldNotify(
            capability,
            100));

    EXPECT_TRUE(
        limiter.ShouldNotify(
            data,
            100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            capability,
            100));

    EXPECT_FALSE(
        limiter.ShouldNotify(
            data,
            100));
}