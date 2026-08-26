#include <any>

#include <gtest/gtest.h>

#include "products/printer_a/CapabilityItem/PrinterACapabilityComparators.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

TEST(
    CapabilityComparatorTest,
    EnvironmentTemperatureOnlyChanged)
{
    rim::EnvironmentCapability oldCap{};

    oldCap.temperature =
        300.15;

    oldCap.humidity =
        60.0;

    auto newCap =
        oldCap;

    newCap.temperature =
        300.16;

    EXPECT_TRUE(
        rim::CompareEnvironment(
            std::any(oldCap),
            std::any(newCap)));
}

TEST(
    CapabilityComparatorTest,
    EnvironmentHumidityOnlyChanged)
{
    rim::EnvironmentCapability oldCap{};

    oldCap.temperature =
        300.15;

    oldCap.humidity =
        60.0;

    auto newCap =
        oldCap;

    newCap.humidity =
        61.0;

    EXPECT_TRUE(
        rim::CompareEnvironment(
            std::any(oldCap),
            std::any(newCap)));
}

TEST(
    CapabilityComparatorTest,
    EnvironmentSameValue)
{
    rim::EnvironmentCapability oldCap{};

    oldCap.temperature =
        300.15;

    oldCap.humidity =
        60.0;

    auto newCap =
        oldCap;

    EXPECT_FALSE(
        rim::CompareEnvironment(
            std::any(oldCap),
            std::any(newCap)));
}

TEST(
    CapabilityComparatorTest,
    PrintReadyChanged)
{
    rim::PrintReadyCapability oldCap{};

    oldCap.ready =
        false;

    auto newCap =
        oldCap;

    newCap.ready =
        true;

    EXPECT_TRUE(
        rim::ComparePrintReady(
            std::any(oldCap),
            std::any(newCap)));
}

TEST(
    CapabilityComparatorTest,
    PrintReadySameValue)
{
    rim::PrintReadyCapability oldCap{};

    oldCap.ready =
        true;

    auto newCap =
        oldCap;

    EXPECT_FALSE(
        rim::ComparePrintReady(
            std::any(oldCap),
            std::any(newCap)));
}