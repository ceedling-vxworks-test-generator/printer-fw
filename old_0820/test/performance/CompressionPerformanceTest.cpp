#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <list>

#include "printer_a.h"

#include "CompressionPolicyResolver.hpp"
#include "QueuePolicy.hpp"
#include "PublisherInput.hpp"

TEST(
    CompressionPerformanceTest,
    ResolverThroughput)
{
    constexpr std::uint32_t
        kIterations = 1000000;

    rim::NotificationTarget target
    {
        rim::NotificationTargetType::
            Capability,

        RI_CAPABILITY_ENVIRONMENT
    };

    const auto start =
        std::chrono::steady_clock::now();

    std::size_t keepLatest = 0;

    for (std::uint32_t i = 0;
         i < kIterations;
         ++i)
    {
        const auto policy =
            rim::CompressionPolicyResolver::
                Resolve(
                    target);

        if (policy ==
            rim::CompressionPolicy::
                KeepLatest)
        {
            ++keepLatest;
        }
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
        << "\n=== CompressionPolicyResolver ===\n"
        << "Iterations  : "
        << kIterations
        << '\n'
        << "KeepLatest  : "
        << keepLatest
        << '\n'
        << "Elapsed(us) : "
        << elapsedUs
        << '\n'
        << "Resolve/sec : "
        << static_cast<std::uint64_t>(
               static_cast<double>(
                   kIterations)
               * 1000000.0
               / elapsedUs)
        << '\n';
}

#include <list>

#include "QueuePolicy.hpp"
#include "PublisherInput.hpp"

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
                {
                    rim::NotificationTargetType::
                        Capability,

                    RI_CAPABILITY_ENVIRONMENT
                },

                rim::EventPriority::
                    Normal
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
                {
                    rim::NotificationTargetType::
                        Capability,

                    RI_CAPABILITY_ENVIRONMENT
                },

                rim::EventPriority::
                    Normal
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
