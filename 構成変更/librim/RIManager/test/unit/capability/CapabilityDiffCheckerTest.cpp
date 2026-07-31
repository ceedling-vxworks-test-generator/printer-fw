#include <gtest/gtest.h>

#include "ErrorInfo.hpp"
#include "JobCapability.hpp"
#include "CapabilityDiffChecker.hpp"

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

TEST(
    CapabilityDiffCheckerTest,
    DetectConsumableChange)
{
    rim::ConsumableCapability lhs{};
    lhs.stapleLevel = 100;

    rim::ConsumableCapability rhs{};
    rhs.stapleLevel = 80;

    EXPECT_TRUE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

TEST(
    CapabilityDiffCheckerTest,
    DetectErrorChange)
{
    rim::ErrorCapability lhs{};

    lhs.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    rim::ErrorCapability rhs{};

    EXPECT_TRUE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

TEST(
    CapabilityDiffCheckerTest,
    DetectJobChange)
{
    rim::JobCapability lhs{};

    rim::JobCapability rhs{};

    rhs.jobActive = true;
    rhs.jobId = 123;

    EXPECT_TRUE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

TEST(
    CapabilityDiffCheckerTest,
    DetectPrintReadyChange)
{
    rim::PrintReadyCapability lhs{};

    rim::PrintReadyCapability rhs{};

    rhs.ready = true;

    EXPECT_TRUE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

TEST(
    CapabilityDiffCheckerTest,
    DetectNoErrorChange)
{
    rim::ErrorCapability lhs{};

    lhs.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    rim::ErrorCapability rhs =
        lhs;

    EXPECT_FALSE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

TEST(
    CapabilityDiffCheckerTest,
    DetectNoJobChange)
{
    rim::JobCapability lhs{};

    lhs.jobActive = true;
    lhs.jobId = 123;

    rim::JobCapability rhs =
        lhs;

    EXPECT_FALSE(
        rim::CapabilityDiffChecker::
            HasChanged(
                lhs,
                rhs));
}

