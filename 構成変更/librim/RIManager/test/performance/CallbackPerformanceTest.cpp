#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "PerformanceTestFixture.hpp"

namespace
{

constexpr int kMessageCount =
    10000;

std::atomic<int>
    g_receivedCount{0};

void OnEnvironment(
    uint64_t subscriptionId,
    const void* capability,
    void* userData)
{
    (void)subscriptionId;
    (void)capability;
    (void)userData;

    ++g_receivedCount;
}

} // namespace

TEST_F(
    PerformanceTestFixture,
    EnvironmentCallbackThroughput)
{
    g_receivedCount = 0;

    uint64_t subscriptionId{};

    ASSERT_EQ(
        RIManager_SubscribeEnvironment(
            handle_,
            OnEnvironment,
            nullptr,
            &subscriptionId),
        RI_SUCCESS);

    const auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kMessageCount;
         ++i)
    {
        ASSERT_EQ(
            RIManager_TestInjectTemperature(
                handle_,
                static_cast<double>(i)),
            RI_SUCCESS);
    }

    std::this_thread::sleep_for(
        std::chrono::seconds(5));

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                end - start);

    const auto received =
        g_receivedCount.load();

    std::cout
        << "[PERF] Injected : "
        << kMessageCount
        << std::endl;

    std::cout
        << "[PERF] Received : "
        << received
        << std::endl;

    std::cout
        << "[PERF] Elapsed  : "
        << elapsedMs.count()
        << " ms"
        << std::endl;

    std::cout
        << "[PERF] Callback/sec : "
        << (1000.0 * received)
            / std::max(
                int64_t{1},
                elapsedMs.count())
        << std::endl;

    EXPECT_GT(
        received,
        0);

    ASSERT_EQ(
        RIManager_Unsubscribe(
            handle_,
            subscriptionId),
        RI_SUCCESS);
}
