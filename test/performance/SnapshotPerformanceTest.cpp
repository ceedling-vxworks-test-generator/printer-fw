#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "DomainStorageRegistry.hpp"

// TEST(
//     SnapshotPerformanceTest,
//     BuildSnapshot10000Times)
// {
//     rim::DomainStorageRegistry domainStore;

//     rim::RIMSnapshotManager reader(
//         domainStore);

//     constexpr std::size_t kIteration =
//         10000;

//     const auto start =
//         std::chrono::steady_clock::now();

//     for (std::size_t i = 0;
//          i < kIteration;
//          ++i)
//     {
//         [[maybe_unused]]
//         auto snapshot =
//             reader.Read();
//     }

//     const auto elapsed =
//         std::chrono::steady_clock::now() -
//         start;

//     const auto us =
//         std::chrono::duration_cast<
//             std::chrono::microseconds>(
//                 elapsed);

//     std::cout
//         << "[PERF] Snapshot x "
//         << kIteration
//         << " : "
//         << us.count()
//         << " us"
//         << std::endl;
// }

#if 0
#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <iostream>

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

TEST(
    SnapshotPerformanceTest,
    FindScale)
{
    constexpr std::size_t kItemCounts[] =
    {
        100,
        1000,
        10000,
        100000
    };

    constexpr std::uint32_t kIterations =
        1000000;

    for (const auto itemCount :
         kItemCounts)
    {
        rim::RIMSnapshot snapshot;

        snapshot.items.reserve(
            itemCount);

        for (std::size_t i = 0;
             i < itemCount;
             ++i)
        {
            rim::RIMDataItem item{};

            item.id =
                static_cast<RIDataId>(
                    i + 1);

            item.valueType =
                rim::ValueType::kDouble;

            item.value =
                rim::RIMValueFactory::
                    CreateDouble(
                        static_cast<double>(
                            i));

            snapshot.items.push_back(
                std::move(item));
        }

        const auto targetId =
            static_cast<RIDataId>(
                itemCount);

        double value{};

        const auto start =
            std::chrono::steady_clock::now();

        for (std::uint32_t i = 0;
             i < kIterations;
             ++i)
        {
            ASSERT_TRUE(
                snapshot.TryGetDouble(
                    targetId,
                    value));
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

        const auto throughput =
            static_cast<double>(
                kIterations)
            * 1000000.0
            / static_cast<double>(
                elapsedUs);

        std::cout
            << "\n=== Snapshot Find Scale ===\n"
            << "Items       : "
            << itemCount
            << '\n'
            << "Iterations  : "
            << kIterations
            << '\n'
            << "Elapsed(us) : "
            << elapsedUs
            << '\n'
            << "Find/sec    : "
            << static_cast<std::uint64_t>(
                   throughput)
            << '\n';
    }
}

TEST(
    SnapshotPerformanceTest,
    FindBestVsWorstCase)
{
    constexpr std::size_t kItems =
        10000;

    constexpr std::uint32_t kIterations =
        1000000;

    rim::RIMSnapshot snapshot;

    for (std::size_t i = 0;
         i < kItems;
         ++i)
    {
        rim::RIMDataItem item{};

        item.id =
            static_cast<RIDataId>(
                i + 1);

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::
                CreateDouble(
                    static_cast<double>(
                        i));

        snapshot.items.push_back(
            std::move(item));
    }

    auto measure =
        [&](RIDataId id)
    {
        double value{};

        const auto start =
            std::chrono::steady_clock::now();

        for (std::uint32_t i = 0;
             i < kIterations;
             ++i)
        {
            snapshot.TryGetDouble(
                id,
                value);
        }

        const auto end =
            std::chrono::steady_clock::now();

        return std::chrono::
            duration_cast
            <
                std::chrono::microseconds
            >
            (
                end - start
            )
            .count();
    };

    const auto best =
        measure(
            static_cast<RIDataId>(
                1));

    const auto worst =
        measure(
            static_cast<RIDataId>(
                kItems));

    std::cout
        << "\n=== Snapshot Search Position ===\n"
        << "BestCase(us)  : "
        << best
        << '\n'
        << "WorstCase(us) : "
        << worst
        << '\n'
        << "Ratio         : "
        << static_cast<double>(
               worst)
               / static_cast<double>(
                   best)
        << '\n';
}
#endif