#include <gtest/gtest.h>

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRules.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "test/support/SnapshotTestHelper.hpp"

TEST(
    PrinterAConsumableRuleTest,
    BuildConsumableCapability)
{
    rim::RIMSnapshot snapshot{};

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kStapleLevel,
        80);

    rim::AddInt32Item(
        snapshot,
        rim::RIMDataId::kTonerLevel,
        50);

    rim::ConsumableCapabilityRule rule;

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::ConsumableCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_EQ(
        cap->stapleLevel,
        80);

    EXPECT_EQ(
        cap->tonerLevel,
        50);
}

TEST(
    PrinterAConsumableRuleTest,
    ReportsItsOwnCapabilityId)
{
    rim::ConsumableCapabilityRule rule;

    EXPECT_EQ(
        rule.Id(),
        rim::kCapConsumable);
}
