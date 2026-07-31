#include <gtest/gtest.h>

#include "CapabilityChangeTracker.hpp"
#include "CapabilityEvaluator.hpp"
#include "CapabilityStore.hpp"
#include "ErrorRepository.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// 値が変わっていなければ配信しない、という RIM の基本方針の確認。
// (生の値が来たときではなく、Capability が変わったときだけ配信する)
//

TEST(
    CapabilityToPublisherNoChangeTest,
    NoChangeWhenSameSnapshotIsEvaluatedTwice)
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

    rim::ErrorRepository errorRepository;

    rim::PrinterACapabilityRuleSet ruleSet(
        errorRepository);

    rim::CapabilityEvaluator evaluator;

    ruleSet.RegisterTo(
        evaluator);

    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    evaluator.Evaluate(
        snapshot,
        store);

    const auto first =
        tracker.Detect(
            store);

    // 初回は全 Capability が「変化」として上がる
    EXPECT_EQ(
        first.Size(),
        ruleSet.CapabilityCount());

    EXPECT_TRUE(
        first.Contains(
            rim::kCapEnvironment));

    // 同じ Snapshot をもう一度評価しても、変化は無い
    evaluator.Evaluate(
        snapshot,
        store);

    EXPECT_TRUE(
        tracker.Detect(
            store)
            .Empty());
}

TEST(
    CapabilityToPublisherNoChangeTest,
    OnlyTheAffectedCapabilityIsReported)
{
    rim::RIMSnapshot snapshot{};

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        300.15);

    rim::ErrorRepository errorRepository;

    rim::PrinterACapabilityRuleSet ruleSet(
        errorRepository);

    rim::CapabilityEvaluator evaluator;

    ruleSet.RegisterTo(
        evaluator);

    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    evaluator.Evaluate(snapshot, store);

    tracker.Detect(store);

    // 温度だけ動かす
    rim::RIMSnapshot changed{};

    rim::AddDoubleItem(
        changed,
        rim::RIMDataId::kTemperatureSensorA,
        305.15);

    evaluator.Evaluate(changed, store);

    const auto changes =
        tracker.Detect(store);

    EXPECT_TRUE(
        changes.Contains(
            rim::kCapEnvironment));

    // 扉も異常も動いていないので印字可否は変わらない
    EXPECT_FALSE(
        changes.Contains(
            rim::kCapPrintReady));
}
