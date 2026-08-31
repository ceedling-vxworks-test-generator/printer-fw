#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>
#include <thread>

#include "PerformanceStatistics.hpp"
#include "PerformanceTestFixture.hpp"

namespace
{

using Clock =
    std::chrono::steady_clock;

struct CallbackContext
{
    std::mutex mutex;

    std::condition_variable cv;

    std::uint64_t callbackCount{0};

    Clock::time_point end;
};

CallbackContext g_context;

void OnEnvironment(
    uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    {
        std::lock_guard<std::mutex>
            lock(
                g_context.mutex);

        ++g_context.callbackCount;

        std::cout
            << "callbackCount="
            << g_context.callbackCount
            << std::endl;

        g_context.end =
            Clock::now();
    }

    g_context.cv.notify_one();
}

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
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            &subscriptionId),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_HUMIDITY_SENSOR,
            80.0),
        RI_SUCCESS);

    //
    // 初期状態
    // Environment = 1
    //
    ASSERT_EQ(
        RIM_SetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            25.0),
        RI_SUCCESS);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(
            250));

    for (int i = 0;
         i < kIterationCount;
         ++i)
    {
        std::uint64_t previousCount{};

        {
            std::lock_guard<std::mutex>
                lock(
                    g_context.mutex);

            previousCount =
                g_context.callbackCount;
        }

        std::cout
            << "iteration="
            << i
            << " previousCount="
            << previousCount
            << std::endl;

        const double temperature =
            (i % 2 == 0)
                ? 60.0   // state=2
                : 25.0;  // state=1

        const auto start =
            Clock::now();

        ASSERT_EQ(
            RIM_SetDouble(
                RI_DATA_TEMPERATURE_SENSOR_A,
                temperature),
            RI_SUCCESS);

        {
            std::unique_lock<std::mutex>
                lock(
                    g_context.mutex);

            const bool received =
                g_context.cv.wait_until(
                    lock,
                    Clock::now() +
                        kTimeout,
                    [&]
                    {
                        return
                            g_context.callbackCount >
                            previousCount;
                    });

            ASSERT_TRUE(
                received)
                << "Timeout waiting for callback";
        }

        const auto latency =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    g_context.end -
                    start);

        latenciesUs.push_back(
            latency.count());

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                120));
    }

    const auto stats =
        perf::CalculateStatistics(
            latenciesUs);

    std::cout
        << "\n[PERF] Environment Callback Latency\n"
        << "  Samples : "
        << latenciesUs.size()
        << '\n'
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
}

} // namespace