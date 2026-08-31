#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <list>

#include "rim_api.h"
#include "QueuePolicy.hpp"
#include "PublisherInput.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    CompressionPerformanceTest,
    EnvironmentCompressionRatio)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    constexpr std::uint32_t
        kEvents = 10000;

    for (std::uint32_t i = 0;
         i < kEvents;
         ++i)
    {
        policy.Insert(
            events,
            {
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::EventPriority::Normal,
                rim::CompressionPolicy::KeepLatest
            });
    }

    std::cout
        << "\n=== Environment Compression ===\n"
        << "Inserted   : "
        << kEvents
        << '\n'
        << "Remaining  : "
        << events.size()
        << '\n'
        << "Compressed : "
        << (kEvents -
            events.size())
        << '\n'
        << "Ratio      : "
        << std::fixed
        << std::setprecision(2)
        << (
            100.0 *
            static_cast<double>(
                kEvents -
                events.size())
            /
            static_cast<double>(
                kEvents))
        << " %\n";
}

TEST(
    CompressionPerformanceTest,
    EnvironmentCompressionThroughput)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    constexpr std::uint32_t
        kEvents = 100000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint32_t i = 0;
         i < kEvents;
         ++i)
    {
        policy.Insert(
            events,
            {
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::EventPriority::Normal,
                rim::CompressionPolicy::KeepLatest
            });
    }

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedUs =
        std::chrono::duration_cast
        <
            std::chrono::microseconds
        >
        (
            end - start
        )
        .count();

    std::cout
        << "\n=== Compression Throughput ===\n"
        << "Inserted      : "
        << kEvents
        << '\n'
        << "Remaining     : "
        << events.size()
        << '\n'
        << "Elapsed(us)   : "
        << elapsedUs
        << '\n'
        << "Insert/sec    : "
        << static_cast<std::uint64_t>(
               static_cast<double>(
                   kEvents)
               * 1000000.0
               / elapsedUs)
        << '\n';
}
