#include <gtest/gtest.h>

#include <chrono>

#include "printer_a.h"
#include "test/performance/PerformanceTestFixture.hpp"

TEST_F(
    PerformanceTestFixture,
    Environment10000Messages)
{
    constexpr int kCount =
        10000;

    const auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kCount;
         ++i)
    {
        ASSERT_EQ(
            RIM_SetDouble(
                RI_DATA_TEMPERATURE_SENSOR_A,
                static_cast<double>(i)),
            RI_SUCCESS);
    }

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsed =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                end - start);

    std::cout
        << "[PERF] Inject "
        << kCount
        << " messages in "
        << elapsed.count()
        << " ms"
        << std::endl;
}
