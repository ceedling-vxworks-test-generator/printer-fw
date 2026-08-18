#include <gtest/gtest.h>

#include "printer_a.h"

#include "NotificationRateLimiter.hpp"

#include "NotificationTargetType.hpp"
#include <thread>

TEST(
    NotificationRateLimiterTest,
    IgnoreRapidNotification)
{
    rim::NotificationRateLimiter
        limiter;

    rim::NotificationTarget
        target
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        };

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
    rim::NotificationRateLimiter
        limiter;

    rim::NotificationTarget
        environment
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        };

    rim::NotificationTarget
        printReady
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_PRINT_READY
        };

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
    rim::NotificationRateLimiter
        limiter;

    rim::NotificationTarget
        target
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        };

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

    rim::NotificationTarget
        environment
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        };

    rim::NotificationTarget
        consumable
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_CONSUMABLE
        };

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
    rim::NotificationRateLimiter
        limiter;

    rim::NotificationTarget
        target
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        };

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