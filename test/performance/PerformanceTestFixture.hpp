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
            RIManager_Create(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Start(),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIManager_Stop(),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Destroy(),
            RI_SUCCESS);
    }
};
