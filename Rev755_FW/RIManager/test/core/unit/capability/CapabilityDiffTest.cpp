#include <gtest/gtest.h>

#include "Product.hpp"

#include "RIMValueFactory.hpp"

TEST(
    CapabilityDiffTest,
    EnvironmentStateChanged)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            2);

    EXPECT_TRUE(
        rim::kEnvironmentCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    EnvironmentStateUnchanged)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    EXPECT_FALSE(
        rim::kEnvironmentCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    EnvironmentNormalToWarning)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            0);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    EXPECT_TRUE(
        rim::kEnvironmentCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    EnvironmentWarningToCritical)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            2);

    EXPECT_TRUE(
        rim::kEnvironmentCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    PrintReadyChanged)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateBool(
            false);

    const auto newValue =
        rim::RIMValueFactory::CreateBool(
            true);

    EXPECT_TRUE(
        rim::kPrintReadyCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    PrintReadyUnchanged)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateBool(
            true);

    const auto newValue =
        rim::RIMValueFactory::CreateBool(
            true);

    EXPECT_FALSE(
        rim::kPrintReadyCapability.diff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    IdentityDiffDetectsBoolChange)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateBool(
            false);

    const auto newValue =
        rim::RIMValueFactory::CreateBool(
            true);

    EXPECT_TRUE(
        rim::IdentityDiff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    IdentityDiffDetectsInt32Change)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            1);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            2);

    EXPECT_TRUE(
        rim::IdentityDiff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    IdentityDiffReturnsFalseForSameInt32)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateInt32(
            2);

    const auto newValue =
        rim::RIMValueFactory::CreateInt32(
            2);

    EXPECT_FALSE(
        rim::IdentityDiff(
            oldValue,
            newValue));
}

TEST(
    CapabilityDiffTest,
    IdentityDiffReturnsFalseForSameBool)
{
    const auto oldValue =
        rim::RIMValueFactory::CreateBool(
            true);

    const auto newValue =
        rim::RIMValueFactory::CreateBool(
            true);

    EXPECT_FALSE(
        rim::IdentityDiff(
            oldValue,
            newValue));
}