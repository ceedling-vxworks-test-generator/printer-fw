#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"
#include "RIMSnapshot.hpp"

#include "PrinterAProductDefinition.hpp"

TEST(
    CapabilityPerformanceTest,
    Evaluate10000Times)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::RIMSnapshot snapshot;

    constexpr std::size_t kIteration =
        10000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::size_t i = 0;
         i < kIteration;
         ++i)
    {
        manager.Evaluate(
            snapshot);
    }

    const auto elapsed =
        std::chrono::steady_clock::now() -
        start;

    const auto us =
        std::chrono::duration_cast<
            std::chrono::microseconds>(
                elapsed);

    const double avgUs =
        static_cast<double>(
            us.count()) /
        static_cast<double>(
            kIteration);

    const double evalPerSec =
        static_cast<double>(
            kIteration) *
        1000000.0 /
        static_cast<double>(
            us.count());

    std::cout
        << "\n[PERF] Capability Evaluation\n"
        << "  Iterations : "
        << kIteration
        << "\n"
        << "  Total      : "
        << us.count()
        << " us\n"
        << "  Avg        : "
        << avgUs
        << " us\n"
        << "  Eval/sec   : "
        << static_cast<std::uint64_t>(
               evalPerSec)
        << "\n";
}
