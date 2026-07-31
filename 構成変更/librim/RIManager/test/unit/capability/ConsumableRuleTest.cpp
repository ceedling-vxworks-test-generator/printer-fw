#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "ConsumableRule.hpp"

TEST(
    ConsumableRuleTest,
    BuildConsumableCapability)
{
    rim::RIMSnapshot
        snapshot{};

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kStapleLevel,
        80);

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kTonerLevel,
        60);

    rim::ConsumableRule
        rule;

    const auto capability =
        rule.Evaluate(
            snapshot);

    EXPECT_EQ(
        capability.stapleLevel,
        80);

    EXPECT_EQ(
        capability.tonerLevel,
        60);
}