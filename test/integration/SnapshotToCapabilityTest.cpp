#include <gtest/gtest.h>

#include "CapabilityEvaluator.hpp"
#include "CapabilityStore.hpp"
#include "ErrorRepository.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// Snapshot -> Capability 生成。
//
// 旧試験は core の CapabilityManager が5種の規則を内蔵している前提だった。
// 現在は Product(PrinterACapabilityRuleSet)が規則を持ち込み、
// Core の CapabilityEvaluator はそれを回すだけである。
// この試験は **その受け渡しが成立していること** の確認にもなっている。
//

TEST(
    SnapshotToCapabilityTest,
    GenerateCapabilities)
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

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kUpperDoorOpen,
        false);

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kRightDoorOpen,
        false);

    rim::AddBoolItem(
        snapshot,
        rim::RIMDataId::kLeftDoorOpen,
        false);

    rim::ErrorRepository errorRepository;

    rim::PrinterACapabilityRuleSet ruleSet(
        errorRepository);

    rim::CapabilityEvaluator evaluator;

    ruleSet.RegisterTo(
        evaluator);

    rim::CapabilityStore store;

    evaluator.Evaluate(
        snapshot,
        store);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        store.TryGet(
            rim::kCapEnvironment,
            payload));

    const auto* environment =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        environment,
        nullptr);

    EXPECT_DOUBLE_EQ(
        environment->temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        environment->humidity,
        60.0);

    ASSERT_TRUE(
        store.TryGet(
            rim::kCapPrintReady,
            payload));

    const auto* printReady =
        payload.As<
            rim::PrintReadyCapability>();

    ASSERT_NE(
        printReady,
        nullptr);

    EXPECT_TRUE(
        printReady->ready);
}

TEST(
    SnapshotToCapabilityTest,
    AllRegisteredCapabilitiesAreGenerated)
{
    // Product が登録した規則の数だけ Capability が生成されること。
    // Core は「いくつあるか」を Product から受け取るだけで、中身を知らない。
    rim::RIMSnapshot snapshot{};

    rim::ErrorRepository errorRepository;

    rim::PrinterACapabilityRuleSet ruleSet(
        errorRepository);

    rim::CapabilityEvaluator evaluator;

    ruleSet.RegisterTo(
        evaluator);

    EXPECT_EQ(
        evaluator.RuleCount(),
        ruleSet.CapabilityCount());

    rim::CapabilityStore store;

    EXPECT_EQ(
        evaluator.Evaluate(
            snapshot,
            store),
        ruleSet.CapabilityCount());
}
