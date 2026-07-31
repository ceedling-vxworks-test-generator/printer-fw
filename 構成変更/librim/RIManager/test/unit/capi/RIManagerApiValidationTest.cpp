#include <gtest/gtest.h>

#include "rim_api.h"

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"

TEST(
    RIManagerApiValidationTest,
    GetEnvironmentInvalidHandle)
{
    rim::EnvironmentCapability cap{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetEnvironment(
            nullptr,
            &cap));
}

TEST(
    RIManagerApiValidationTest,
    GetErrorInvalidHandle)
{
    rim::ErrorCapability capability{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetError(
            nullptr,
            &capability));
}

TEST(
    RIManagerApiValidationTest,
    GetPrintReadyInvalidHandle)
{
    rim::PrintReadyCapability capability{};

    EXPECT_EQ(
        RI_INVALID_PARAMETER,
        RIManager_GetPrintReady(
            nullptr,
            &capability));
}