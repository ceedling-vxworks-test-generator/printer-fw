#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "PerformanceTestFixture.hpp"

namespace
{

std::atomic<std::uint64_t>
    g_receivedCount{0};

void OnEnvironment(
    uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;

    ++g_receivedCount;
}

} // namespace

TEST_F(
    PerformanceTestFixture,
    NotificationDeliveryStatistics)
{
    constexpr std::uint64_t kMessageCount =
        10000;

    constexpr auto kDrainTime =
        std::chrono::seconds(5);

    g_receivedCount = 0;

    uint64_t subscriptionId{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            &subscriptionId),
        RI_SUCCESS);

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint64_t i = 0;
         i < kMessageCount;
         ++i)
    {
        ASSERT_EQ(
            RIM_SetDouble(
                RI_DATA_TEMPERATURE_SENSOR_A,
                static_cast<double>(i)),
            RI_SUCCESS);
    }

    std::this_thread::sleep_for(
        kDrainTime);

    const auto elapsed =
        std::chrono::steady_clock::now()
        - start;

    const auto delivered =
        g_receivedCount.load();

    const double deliveryRatio =
        static_cast<double>(
            delivered) *
        100.0 /
        static_cast<double>(
            kMessageCount);

    std::cout
        << "\n[PERF] Notification Delivery Statistics\n"
        << "  Injected       : "
        << kMessageCount
        << "\n"
        << "  Delivered      : "
        << delivered
        << "\n"
        << "  Delivery Ratio : "
        << deliveryRatio
        << " %\n"
        << "  Elapsed        : "
        << std::chrono::duration_cast<
               std::chrono::milliseconds>(
                   elapsed)
               .count()
        << " ms\n";

    ASSERT_EQ(
        RIM_Unsubscribe(
            subscriptionId),
        RI_SUCCESS);
}