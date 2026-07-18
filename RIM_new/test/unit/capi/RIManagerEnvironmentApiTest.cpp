#include <gtest/gtest.h>

#include "rim_api.h"

#include "capability/EnvironmentCapability.hpp"

TEST(
    RIManagerEnvironmentApiTest,
    InvalidHandle)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetEnvironment(
            nullptr,
            &cap));
}