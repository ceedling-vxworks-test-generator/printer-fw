#include <gtest/gtest.h>

#include <any>

#include "CapabilityItem/PrinterACapabilityComparators.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

TEST(
    CapabilityComparatorTest,
    DetectEnvironmentChange)
{
    rim::EnvironmentCapability oldCap{};

    oldCap.temperature = 300.15;
    oldCap.humidity = 60.0;

    rim::EnvironmentCapability newCap{};

    newCap.temperature = 301.15;
    newCap.humidity = 60.0;

    EXPECT_TRUE(
        rim::CompareEnvironment(
            oldCap,
            newCap));
}

// TEST(
//     CapabilityComparatorTest,
//     DetectConsumableChange)
// {
//     rim::ConsumableCapability lhs{};

//     lhs.stapleLevel = 100;
//     lhs.tonerLevel = 80;

//     rim::ConsumableCapability rhs{};

//     rhs.stapleLevel = 80;
//     rhs.tonerLevel = 80;

//     EXPECT_TRUE(
//         rim::CompareConsumable(
//             lhs,
//             rhs));
// }

// TEST(
//     CapabilityComparatorTest,
//     DetectJobChange)
// {
//     rim::JobCapability lhs{};

//     rim::JobCapability rhs{};

//     rhs.jobActive = true;
//     rhs.jobId = 123;

//     EXPECT_TRUE(
//         rim::CompareJob(
//             lhs,
//             rhs));
// }

TEST(
    CapabilityComparatorTest,
    DetectPrintReadyChange)
{
    rim::PrintReadyCapability lhs{};

    rim::PrintReadyCapability rhs{};

    rhs.ready = true;

    EXPECT_TRUE(
        rim::ComparePrintReady(
            lhs,
            rhs));
}

// TEST(
//     CapabilityComparatorTest,
//     DetectNoJobChange)
// {
//     rim::JobCapability lhs{};

//     lhs.jobActive = true;
//     lhs.jobId = 123;

//     rim::JobCapability rhs =
//         lhs;

//     EXPECT_FALSE(
//         rim::CompareJob(
//             lhs,
//             rhs));
// }

TEST(
    CapabilityComparatorTest,
    DetectNoEnvironmentChange)
{
    rim::EnvironmentCapability lhs{};

    lhs.temperature = 300.15;
    lhs.humidity = 60.0;

    rim::EnvironmentCapability rhs =
        lhs;

    EXPECT_FALSE(
        rim::CompareEnvironment(
            lhs,
            rhs));
}

TEST(
    CapabilityComparatorTest,
    DetectNoPrintReadyChange)
{
    rim::PrintReadyCapability lhs{};

    lhs.ready = true;

    rim::PrintReadyCapability rhs =
        lhs;

    EXPECT_FALSE(
        rim::ComparePrintReady(
            lhs,
            rhs));
}

// TEST(
//     CapabilityComparatorTest,
//     DetectNoConsumableChange)
// {
//     rim::ConsumableCapability lhs{};

//     lhs.stapleLevel = 100;
//     lhs.tonerLevel = 80;

//     rim::ConsumableCapability rhs =
//         lhs;

//     EXPECT_FALSE(
//         rim::CompareConsumable(
//             lhs,
//             rhs));
// }