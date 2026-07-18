#include <gtest/gtest.h>

#include "publisher/CapabilityDiffChecker.hpp"

TEST(
    CapabilityDiffCheckerTest,
    DetectEnvironmentChange)
{
    rim::EnvironmentCapability oldCap{};

    oldCap.temperature = 300.15;
    oldCap.humidity = 60.0;

    rim::EnvironmentCapability newCap{};

    newCap.temperature = 301.15;
    newCap.humidity = 60.0;

    EXPECT_TRUE(
        rim::CapabilityDiffChecker::HasChanged(
            oldCap,
            newCap));
}