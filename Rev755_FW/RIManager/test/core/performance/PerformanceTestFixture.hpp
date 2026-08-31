#pragma once

#include <gtest/gtest.h>

#include "rim_api.h"

class PerformanceTestFixture
    : public ::testing::Test
{
protected:

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
};