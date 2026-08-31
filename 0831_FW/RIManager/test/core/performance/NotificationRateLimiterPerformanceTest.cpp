#include <gtest/gtest.h>

#include <chrono>

#include "rim_api.h"

#include "NotificationRateLimiter.hpp"

#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    NotificationRateLimiterPerformanceTest,
    Throughput)
{
    constexpr std::size_t iterations =
        1000000;

    rim::NotificationRateLimiter
        limiter;

    rim::RIMId target =
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    auto start =
        std::chrono::steady_clock::now();

    std::size_t allowed = 0;

    for (std::size_t i = 0;
         i < iterations;
         ++i)
    {
        if (limiter.ShouldNotify(
                target,100))
        {
            ++allowed;
        }
    }

    auto end =
        std::chrono::steady_clock::now();

    const auto elapsedUs =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                end - start)
                .count();

    std::cout
        << std::endl
        << "=== NotificationRateLimiter ==="
        << std::endl
        << "Iterations : "
        << iterations
        << std::endl
        << "Allowed    : "
        << allowed
        << std::endl
        << "Elapsed(us): "
        << elapsedUs
        << std::endl
        << "Calls/sec  : "
        << static_cast<uint64_t>(
            (iterations * 1000000.0)
            / elapsedUs)
        << std::endl;
}

TEST(
    NotificationRateLimiterPerformanceTest,
    SuppressionEffect)
{
    constexpr std::size_t publishCount =
        100000;

    rim::NotificationRateLimiter
        limiter;

    rim::RIMId target = 
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    std::size_t allowed = 0;

    auto start =
        std::chrono::steady_clock::now();

    for (std::size_t i = 0;
         i < publishCount;
         ++i)
    {
        if (limiter.ShouldNotify(
                target,100))
        {
            ++allowed;
        }
    }

    auto end =
        std::chrono::steady_clock::now();

    const auto elapsedUs =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                end - start)
                .count();

    std::cout
        << std::endl
        << "=== Notification Suppression ==="
        << std::endl
        << "Publishes : "
        << publishCount
        << std::endl
        << "Allowed   : "
        << allowed
        << std::endl
        << "Blocked   : "
        << (publishCount - allowed)
        << std::endl
        << "Ratio     : "
        << (100.0 *
            (publishCount - allowed)
            / publishCount)
        << " %"
        << std::endl
        << "Elapsed(us): "
        << elapsedUs
        << std::endl;
}