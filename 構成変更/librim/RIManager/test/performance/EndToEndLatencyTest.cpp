#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "PerformanceStatistics.hpp"
#include "PerformanceTestFixture.hpp"

namespace
{

using Clock =
    std::chrono::steady_clock;

std::atomic<bool>
    g_notified{false};

Clock::time_point
    g_end;

void OnEnvironment(
    uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    g_end =
        Clock::now();

    g_notified = true;
}

} // namespace

TEST_F(
    PerformanceTestFixture,
    EnvironmentCallbackLatency)
{
    constexpr int kIterationCount =
        20;

    constexpr auto kTimeout =
        std::chrono::seconds(1);

    std::vector<int64_t>
        latenciesUs;

    latenciesUs.reserve(
        kIterationCount);

    uint64_t subscriptionId{};

    ASSERT_EQ(
        RIManager_SubscribeEnvironment(
            handle_,
            OnEnvironment,
            nullptr,
            &subscriptionId),
        RI_SUCCESS);

    for (int i = 1;
         i <= kIterationCount;
         ++i)
    {
        g_notified = false;

        const auto start =
            Clock::now();

        ASSERT_EQ(
            RIManager_TestInjectTemperature(
                handle_,
                static_cast<double>(i)),
            RI_SUCCESS);

        const auto deadline =
            Clock::now() + kTimeout;

        while (!g_notified)
        {
            if (Clock::now() >= deadline)
            {
                FAIL()
                    << "Timeout waiting for callback";
            }

            std::this_thread::sleep_for(
                std::chrono::microseconds(
                    50));
        }

        const auto latency =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    g_end - start);

        latenciesUs.push_back(
            latency.count());
    }

    const auto stats =
        perf::CalculateStatistics(
            latenciesUs);

    std::cout
        << "\n[PERF] Environment Callback Latency\n"
        << "  Samples : "
        << latenciesUs.size()
        << "\n"
        << "  Min      : "
        << stats.min
        << " us\n"
        << "  Avg      : "
        << stats.avg
        << " us\n"
        << "  P95      : "
        << stats.p95
        << " us\n"
        << "  P99      : "
        << stats.p99
        << " us\n"
        << "  Max      : "
        << stats.max
        << " us\n";

    ASSERT_EQ(
        RIManager_Unsubscribe(
            handle_,
            subscriptionId),
        RI_SUCCESS);
}
