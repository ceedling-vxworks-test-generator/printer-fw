#include <gtest/gtest.h>

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRules.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "test/support/SnapshotTestHelper.hpp"

TEST(
    PrinterAJobRuleTest,
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

    rim::JobCapabilityRule rule;

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::JobCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_TRUE(
        cap->jobActive);

    EXPECT_EQ(
        cap->jobId,
        123);
}

TEST(
    PrinterAJobRuleTest,
    ReportsItsOwnCapabilityId)
{
    rim::JobCapabilityRule rule;

    EXPECT_EQ(
        rule.Id(),
        rim::kCapJob);
}
