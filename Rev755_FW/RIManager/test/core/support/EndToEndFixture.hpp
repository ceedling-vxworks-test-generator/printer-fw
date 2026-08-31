#pragma once

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

class EndToEndFixture
    : public ::testing::Test
{
protected:

    static constexpr auto
        kWaitTime =
            std::chrono::milliseconds(
                100);

    void SetUp() override
    {
        ASSERT_EQ(
            RIM_Create(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIM_Start(),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIM_Stop(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIM_Destroy(),
            RI_SUCCESS);
    }

    void WaitRoute() const
    {
        std::this_thread::sleep_for(
            kWaitTime);
    }
};
