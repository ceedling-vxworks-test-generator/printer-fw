#include <gtest/gtest.h>

#include <cstdint>

#include "CapabilityEvaluator.hpp"
#include "CapabilityStore.hpp"
#include "CapabilityWorker.hpp"
#include "ICapabilityRule.hpp"
#include "PublisherInputQueue.hpp"

#include "test/support/SnapshotTestHelper.hpp"

//
// Snapshot -> Capability 生成 -> 変化検知 -> 配信キュー、の一段。
//
// 旧試験は CapabilityManager と MachineCapabilityStore(いずれも製品の
// Capability 型を直接持つ)を組み立てていたため、Core の試験なのに機種を
// 知っている必要があった。ここでは試験用のダミー規則を1つ登録するだけで済む。
//

namespace
{

struct SampleCapability
{
    double value{};
};

constexpr rim::CapabilityId kSampleId = 0;

// 温度をそのまま Capability にするだけの試験用規則。
class SampleRule final : public rim::ICapabilityRule
{
public:

    rim::CapabilityId Id() const override
    {
        return kSampleId;
    }

    bool Evaluate(
        const rim::RIMSnapshot& snapshot,
        rim::CapabilityPayload& out) const override
    {
        SampleCapability cap{};

        snapshot.TryGetDouble(
            rim::RIMDataId::kTemperatureSensorA,
            cap.value);

        out = rim::CapabilityPayload::From(
            cap);

        return true;
    }
};

// 何も生成しない規則(生成不可のときの扱いを見るため)
class SilentRule final : public rim::ICapabilityRule
{
public:

    rim::CapabilityId Id() const override
    {
        return 1;
    }

    bool Evaluate(
        const rim::RIMSnapshot&,
        rim::CapabilityPayload&) const override
    {
        return false;
    }
};

rim::RIMSnapshot MakeSnapshot(
    double temperature)
{
    rim::RIMSnapshot snapshot{};

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        temperature);

    return snapshot;
}

}

TEST(
    CapabilityWorkerTest,
    ExecuteOnce)
{
    rim::CapabilityInputQueue queue;
    rim::CapabilityStore      store;
    rim::CapabilityEvaluator  evaluator;
    rim::PublisherInputQueue  publisherQueue;

    SampleRule rule;

    evaluator.Register(
        &rule);

    rim::CapabilityWorker worker(
        queue,
        evaluator,
        store,
        publisherQueue);

    queue.Push(
        MakeSnapshot(300.15));

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::CapabilityInputQueue queue;
    rim::CapabilityStore      store;
    rim::CapabilityEvaluator  evaluator;
    rim::PublisherInputQueue  publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        evaluator,
        store,
        publisherQueue);

    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    PublishEventGenerated)
{
    rim::CapabilityInputQueue queue;
    rim::CapabilityStore      store;
    rim::CapabilityEvaluator  evaluator;
    rim::PublisherInputQueue  publisherQueue;

    SampleRule rule;

    evaluator.Register(
        &rule);

    rim::CapabilityWorker worker(
        queue,
        evaluator,
        store,
        publisherQueue);

    queue.Push(
        MakeSnapshot(300.15));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityChangeList changes{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            changes));

    EXPECT_TRUE(
        changes.Contains(
            kSampleId));
}

TEST(
    CapabilityWorkerTest,
    UnchangedSnapshotDoesNotPublish)
{
    // RIM は「生の値が来たとき」ではなく「Capability が変わったとき」に配信する。
    // 同じ値を2回流しても2回目は配信されない。
    rim::CapabilityInputQueue queue;
    rim::CapabilityStore      store;
    rim::CapabilityEvaluator  evaluator;
    rim::PublisherInputQueue  publisherQueue;

    SampleRule rule;

    evaluator.Register(
        &rule);

    rim::CapabilityWorker worker(
        queue,
        evaluator,
        store,
        publisherQueue);

    queue.Push(
        MakeSnapshot(300.15));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityChangeList changes{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            changes));

    // 同じ値をもう一度
    queue.Push(
        MakeSnapshot(300.15));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_FALSE(
        publisherQueue.TryPop(
            changes));
}

TEST(
    CapabilityWorkerTest,
    RuleThatProducesNothingIsSkipped)
{
    rim::CapabilityInputQueue queue;
    rim::CapabilityStore      store;
    rim::CapabilityEvaluator  evaluator;
    rim::PublisherInputQueue  publisherQueue;

    SilentRule rule;

    evaluator.Register(
        &rule);

    rim::CapabilityWorker worker(
        queue,
        evaluator,
        store,
        publisherQueue);

    queue.Push(
        MakeSnapshot(300.15));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    // 生成されていないので配信対象も無い
    rim::CapabilityChangeList changes{};

    EXPECT_FALSE(
        publisherQueue.TryPop(
            changes));
}
