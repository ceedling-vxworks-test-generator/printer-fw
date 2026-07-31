#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "rim_api.h"

#include "DataItem/PrinterADataIds.hpp"

TEST(
    ThroughputTest,
    Environment10000Messages)
{
    constexpr int kCount =
        10000;

    RIM_HANDLE handle{};

    ASSERT_EQ(
        RIManager_Create(
            &handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Start(
            handle),
        RI_SUCCESS);

    const auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kCount;
         ++i)
    {
        ASSERT_EQ(
            RIManager_TestInjectDouble(
                handle,
                rim::ToDataId(
                    rim::RIMDataId::kTemperatureSensorA),
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
        RIManager_Stop(
            handle),
        RI_SUCCESS);

    ASSERT_EQ(
        RIManager_Destroy(
            handle),
        RI_SUCCESS);
}
