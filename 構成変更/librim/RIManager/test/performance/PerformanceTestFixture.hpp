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
            RIManager_Create(
                &handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Start(
                handle_),
            RI_SUCCESS);
    }

    void TearDown() override
    {
        ASSERT_EQ(
            RIManager_Stop(
                handle_),
            RI_SUCCESS);

        ASSERT_EQ(
            RIManager_Destroy(
                handle_),
            RI_SUCCESS);
    }

    RIM_HANDLE
        handle_{};
};