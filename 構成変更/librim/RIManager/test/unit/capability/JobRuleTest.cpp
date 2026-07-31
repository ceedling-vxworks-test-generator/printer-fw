#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "JobRule.hpp"
#include "RIMSnapshot.hpp"

TEST(
    JobRuleTest,
    BuildJobCapability)
{
    rim::RIMSnapshot snapshot{};

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kJobActive,
        true);

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kJobId,
        123);

    rim::JobRule rule;

    const auto capability =
        rule.Evaluate(
            snapshot);

    EXPECT_TRUE(
        capability.jobActive);

    EXPECT_EQ(
        capability.jobId,
        123);
}

TEST(
    JobRuleTest,
    BuildInactiveJobCapability)
{
    rim::RIMSnapshot snapshot{};

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kJobActive,
        false);

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kJobId,
        0);

    rim::JobRule rule;

    const auto capability =
        rule.Evaluate(
            snapshot);

    EXPECT_FALSE(
        capability.jobActive);

    EXPECT_EQ(
        capability.jobId,
        0);
}