#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
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

    bool notified{false};

    Clock::time_point end;
};

void OnEnvironment(
    uint64_t subscriptionId,
    const void* notificationMessage,
    void* userData)
{
    (void)subscriptionId;
    (void)notificationMessage;

    auto* context =
        static_cast<CallbackContext*>(
            userData);

    {
        std::lock_guard<std::mutex> lock(
            context->mutex);

        context->end =
            Clock::now();

        context->notified =
            true;
    }

    context->cv.notify_one();
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

    CallbackContext context;

    uint64_t subscriptionId{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            &context,
            &subscriptionId),
        RI_SUCCESS);

    //
    // Warmup
    //
    for (int i = 0;
         i < 3;
         ++i)
    {
        ASSERT_EQ(
            RIM_TestInjectTemperature(
                1000.0 + i),
            RI_SUCCESS);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(
                10));
    }

    for (int i = 1;
         i <= kIterationCount;
         ++i)
    {
        {
            std::lock_guard<std::mutex>
                lock(
                    context.mutex);

            context.notified =
                false;
        }

        const auto start =
            Clock::now();

        ASSERT_EQ(
            RIM_TestInjectTemperature(
                static_cast<double>(
                    i)),
            RI_SUCCESS);

        {
            std::unique_lock<std::mutex>
                lock(
                    context.mutex);

            const bool received =
                context.cv.wait_until(
                    lock,
                    Clock::now() +
                        kTimeout,
                    [&]
                    {
                        return context
                            .notified;
                    });

            ASSERT_TRUE(
                received)
                << "Timeout waiting "
                   "for callback";
        }

        const auto latency =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                context.end -
                start);

        latenciesUs.push_back(
            latency.count());
    }

    const auto stats =
        perf::CalculateStatistics(
            latenciesUs);

    std::cout
        << "\n[PERF] Environment "
           "Callback Latency\n"
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

    ASSERT_EQ(
        RIM_Unsubscribe(
            subscriptionId),
        RI_SUCCESS);
}