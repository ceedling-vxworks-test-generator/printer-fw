#include <gtest/gtest.h>

#include <chrono>

#include "rim_api.h"

#include "PublisherInputQueue.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    PublisherInputQueuePerformanceTest,
    EnvironmentCompressionPushScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        50000
    };


    for (auto count : counts)
    {
        rim::PublisherInputQueue queue;

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            queue.Push(
            {
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::EventPriority::Normal,
                rim::CompressionPolicy::KeepLatest
            });
        }

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[EnvironmentCompression]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}
TEST(
    PublisherInputQueuePerformanceTest,
    JobPushScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        50000
    };

    for (auto count : counts)
    {
        rim::PublisherInputQueue queue;

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            queue.Push(
            {
                test::CapabilityTarget(RI_CAPABILITY_JOB),
                rim::EventPriority::Normal,
                rim::CompressionPolicy::None
            });
        }

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[JobNotification]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    PublisherInputQueuePerformanceTest,
    CompressionEffect)
{
    rim::PublisherInputQueue queue;

    for (std::size_t i = 0;
         i < 10000;
         ++i)
    {
        queue.Push(
        {
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::EventPriority::Normal,
            rim::CompressionPolicy::KeepLatest
        });
    }

    std::size_t count = 0;

    rim::PublisherInput input;

    while (queue.TryPop(input))
    {
        ++count;
    }

    std::cout
        << "[CompressionEffect]"
        << " remaining="
        << count
        << std::endl;
}