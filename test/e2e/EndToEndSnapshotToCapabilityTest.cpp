#include <gtest/gtest.h>

#include "CapabilityEvaluator.hpp"
#include "CapabilityStore.hpp"
#include "ErrorRepository.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// Snapshot から Product の規則一式を通して Capability が揃うところまで。
//
// 旧試験は EnvironmentRule / PrintReadyRule を個別に new して呼んでいたが、
// 規則の生成と登録は Product(PrinterACapabilityRuleSet)の責務になったため、
// 実運用と同じ組み立てで確認する。
//

TEST(
    EndToEndSnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        30.0);

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kHumiditySensor,
        40.0);

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

    const auto* env =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        env,
        nullptr);

    EXPECT_DOUBLE_EQ(
        env->temperature,
        30.0);

    EXPECT_DOUBLE_EQ(
        env->humidity,
        40.0);

    ASSERT_TRUE(
        store.TryGet(
            rim::kCapPrintReady,
            payload));

    const auto* ready =
        payload.As<
            rim::PrintReadyCapability>();

    ASSERT_NE(
        ready,
        nullptr);

    EXPECT_TRUE(
        ready->ready);
}

TEST(
    EndToEndSnapshotToCapabilityTest,
    ErrorBlocksPrintReady)
{
    rim::RIMSnapshot snapshot{};

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

    errorRepository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

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
            rim::kCapPrintReady,
            payload));

    const auto* ready =
        payload.As<
            rim::PrintReadyCapability>();

    ASSERT_NE(
        ready,
        nullptr);

    // 扉は閉まっているが異常があるので印字不可
    EXPECT_FALSE(
        ready->ready);
}
