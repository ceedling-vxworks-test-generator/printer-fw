#include <gtest/gtest.h>

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRules.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// Capability 規則は core から products へ移した。
// 規則は ICapabilityRule として Core へ渡り、生成物は CapabilityPayload
// (型消去バイト列)になるため、試験も payload.As<T>() で取り出して確認する。
//

TEST(
    PrinterAEnvironmentRuleTest,
    BuildEnvironmentCapability)
{
    rim::RIMSnapshot snapshot{};

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        300.15);

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kHumiditySensor,
        60.0);

    rim::EnvironmentCapabilityRule rule;

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_DOUBLE_EQ(
        cap->temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        cap->humidity,
        60.0);
}

TEST(
    PrinterAEnvironmentRuleTest,
    ReportsItsOwnCapabilityId)
{
    rim::EnvironmentCapabilityRule rule;

    EXPECT_EQ(
        rule.Id(),
        rim::kCapEnvironment);
}

TEST(
    PrinterAEnvironmentRuleTest,
    MissingItemsLeaveZeroValues)
{
    // Snapshot に何も入っていなくても規則は成立する(既定値のまま)。
    // 値が「無い」ことと「0」であることを区別したい場合は、
    // Capability 側に有効フラグを持たせること。
    rim::RIMSnapshot snapshot{};

    rim::EnvironmentCapabilityRule rule;

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        rule.Evaluate(
            snapshot,
            payload));

    const auto* cap =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_DOUBLE_EQ(
        cap->temperature,
        0.0);

    EXPECT_DOUBLE_EQ(
        cap->humidity,
        0.0);
}
