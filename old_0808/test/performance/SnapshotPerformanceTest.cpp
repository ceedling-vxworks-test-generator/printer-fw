#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "ValueStore.hpp"
#include "RIMSnapshotManager.hpp"

TEST(
    SnapshotPerformanceTest,
    BuildSnapshot10000Times)
{
    rim::ValueStore store;

    rim::RIMSnapshotManager reader(
        store);

    constexpr std::size_t kIteration =
        10000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::size_t i = 0;
         i < kIteration;
         ++i)
    {
        [[maybe_unused]]
        auto snapshot =
            reader.Read();
    }

    const auto elapsed =
        std::chrono::steady_clock::now() -
        start;

    const auto us =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                elapsed);

    std::cout
        << "[PERF] Snapshot x "
        << kIteration
        << " : "
        << us.count()
        << " us"
        << std::endl;
}