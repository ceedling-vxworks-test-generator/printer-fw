#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "rim_api.h"

TEST(
    ThroughputTest,
    Environment10000Messages)
{
    constexpr int kCount =
        10000;

    ASSERT_EQ(
        RIM_Create(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Start(),
        RI_SUCCESS);

    const auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kCount;
         ++i)
    {
        ASSERT_EQ(
            RIM_TestInjectTemperature(
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

    ASSERT_EQ(
        RIM_Stop(),
        RI_SUCCESS);

    ASSERT_EQ(
        RIM_Destroy(),
        RI_SUCCESS);
}
