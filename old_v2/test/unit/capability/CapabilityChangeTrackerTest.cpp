#include <gtest/gtest.h>

#include <cstdint>

#include "CapabilityChangeTracker.hpp"
#include "CapabilityStore.hpp"
#include "ICapabilityDiffRule.hpp"

//
// 変化検知。
//
// 旧 CapabilityChangeDetector は previousEnvironment_ / previousJob_ … と
// 製品の Capability 型ごとにメンバを持っていたため、Capability が増えるたびに
// Core の改修が必要だった。本試験は Core の型だけで書けている
// (= Core が製品を知らなくなったことの裏付けでもある)。
//

namespace
{

struct SampleCapability
{
    double       value{};
    std::int32_t count{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

void Put(
    rim::CapabilityStore& store,
    rim::CapabilityId id,
    double value)
{
    SampleCapability cap{};

    cap.value = value;

    store.Store(
        id,
        rim::CapabilityPayload::From(
            cap));
}

// 0.5 以内の差は「変化なし」とみなす規則(誤差許容の例)
class ToleranceDiffRule final : public rim::ICapabilityDiffRule
{
public:

    bool HasChanged(
        const rim::CapabilityPayload& previous,
        const rim::CapabilityPayload& current) const override
    {
        const auto* a =
            previous.As<SampleCapability>();

        const auto* b =
            current.As<SampleCapability>();

        if (a == nullptr ||
            b == nullptr)
        {
            return true;
        }

        const double diff =
            a->value - b->value;

        return diff > 0.5
            || diff < -0.5;
    }
};

}

TEST(
    CapabilityChangeTrackerTest,
    FirstObservationCountsAsChanged)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    Put(store, kSlotA, 1.0);

    const auto changes =
        tracker.Detect(store);

    ASSERT_EQ(
        changes.Size(),
        1U);

    EXPECT_TRUE(
        changes.Contains(
            kSlotA));
}

TEST(
    CapabilityChangeTrackerTest,
    SameValueIsNotReportedTwice)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    Put(store, kSlotA, 1.0);

    tracker.Detect(store);

    // 同じ値をもう一度入れても、変化としては上がらない
    Put(store, kSlotA, 1.0);

    EXPECT_TRUE(
        tracker.Detect(store)
            .Empty());
}

TEST(
    CapabilityChangeTrackerTest,
    ChangedValueIsReported)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    Put(store, kSlotA, 1.0);

    tracker.Detect(store);

    Put(store, kSlotA, 2.0);

    const auto changes =
        tracker.Detect(store);

    ASSERT_EQ(
        changes.Size(),
        1U);

    EXPECT_TRUE(
        changes.Contains(
            kSlotA));
}

TEST(
    CapabilityChangeTrackerTest,
    OnlyChangedSlotsAreReported)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    Put(store, kSlotA, 1.0);
    Put(store, kSlotB, 1.0);

    tracker.Detect(store);

    // A だけ動かす
    Put(store, kSlotA, 9.0);

    const auto changes =
        tracker.Detect(store);

    ASSERT_EQ(
        changes.Size(),
        1U);

    EXPECT_TRUE(
        changes.Contains(
            kSlotA));

    EXPECT_FALSE(
        changes.Contains(
            kSlotB));
}

TEST(
    CapabilityChangeTrackerTest,
    UnstoredSlotsAreNotReported)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    // 何も格納していない
    EXPECT_TRUE(
        tracker.Detect(store)
            .Empty());
}

TEST(
    CapabilityChangeTrackerTest,
    DiffRuleCanAbsorbSmallChanges)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    ToleranceDiffRule rule;

    ASSERT_TRUE(
        tracker.SetDiffRule(
            kSlotA,
            &rule));

    Put(store, kSlotA, 1.0);

    tracker.Detect(store);

    // 許容内の揺れは変化として上がらない
    Put(store, kSlotA, 1.2);

    EXPECT_TRUE(
        tracker.Detect(store)
            .Empty());

    // 許容を超えれば上がる
    Put(store, kSlotA, 3.0);

    EXPECT_TRUE(
        tracker.Detect(store)
            .Contains(
                kSlotA));
}

TEST(
    CapabilityChangeTrackerTest,
    ResetMakesNextObservationChangedAgain)
{
    rim::CapabilityStore store;

    rim::CapabilityChangeTracker tracker;

    Put(store, kSlotA, 1.0);

    tracker.Detect(store);

    tracker.Reset();

    EXPECT_TRUE(
        tracker.Detect(store)
            .Contains(
                kSlotA));
}
