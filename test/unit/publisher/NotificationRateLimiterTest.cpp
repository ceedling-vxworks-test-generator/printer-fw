#include <gtest/gtest.h>

#include "printer_a.h"

#include "NotificationRateLimiter.hpp"

#include "NotificationTargetType.hpp"
#include <thread>
#include "test/support/NotificationTestHelper.hpp"

TEST(
    NotificationRateLimiterTest,
    IgnoreRapidNotification)
{
    rim::NotificationRateLimiter
        limiter;

    rim::NotificationTarget target = 
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

    rim::NotificationTarget environment = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    rim::NotificationTarget printReady = 
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

    rim::NotificationTarget target = 
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

    rim::NotificationTarget environment = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    rim::NotificationTarget consumable = 
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

    rim::NotificationTarget target = 
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

    rim::NotificationTarget capability{
        rim::NotificationTargetType::Capability,
        1};

    rim::NotificationTarget data{
        rim::NotificationTargetType::Data,
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