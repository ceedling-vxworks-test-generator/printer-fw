#include <gtest/gtest.h>

#include <cstdint>

#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityChangeList.hpp"
#include "CapabilityPublisherRegistry.hpp"
#include "CapabilityStore.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"
#include "PublishManager.hpp"
#include "PublisherWorker.hpp"
#include "SubscriberMailbox.hpp"

#include "test/support/CallbackTestHelper.hpp"

//
// 「変化した id の一覧」を受けて、その id の配信器を叩く段。
//
// 旧試験は "Job" / "Error" という文字列で配信器を登録し、
// MachineCapabilityStore.GetJob() のように機種固有の型を直接扱っていた。
// 現在は id と CapabilityPayload だけで書ける。
//

namespace
{

struct SampleCapability
{
    bool         active{};
    std::int32_t id{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

// 配信段一式。試験ごとに同じ組み立てが要るのでまとめてある。
struct Fixture
{
    rim::CapabilityStore              store;
    rim::SubscriberMailbox            mailbox;
    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::ChangeNotifyManager          notifyManager{mailbox, callbackRegistry};
    rim::CapabilityPublisherRegistry  publisherRegistry;
    rim::PublishManager               publishManager{publisherRegistry};
    rim::PublisherInputQueue          queue;
    rim::PublisherWorker              worker{queue, publishManager};
};

}

TEST(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    Fixture f;

    EXPECT_FALSE(
        f.worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    ExecuteOnceWithEmptyChangeList)
{
    Fixture f;

    // 空の一覧でも「1件処理した」ことは真になる(配信対象が無いだけ)
    f.queue.Push(
        rim::CapabilityChangeList{});

    EXPECT_TRUE(
        f.worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    PublishesTheChangedCapability)
{
    Fixture f;

    rim::CapabilityRecorder recorder;

    f.callbackRegistry.Subscribe(
        kSlotA,
        rim::CapabilityRecorder::Callback,
        &recorder);

    rim::StorePublishBinding binding
    {
        &f.store,
        &f.notifyManager,
        kSlotA
    };

    rim::GenericCapabilityPublisher publisher(
        rim::StorePublishBinding::Publish,
        &binding);

    f.publisherRegistry.Register(
        kSlotA,
        &publisher);

    SampleCapability cap{};

    cap.active = true;
    cap.id     = 123;

    f.store.Store(
        kSlotA,
        rim::CapabilityPayload::From(
            cap));

    rim::CapabilityChangeList changes{};

    changes.Add(
        kSlotA);

    f.queue.Push(
        changes);

    ASSERT_TRUE(
        f.worker.ExecuteOnce());

    EXPECT_TRUE(
        recorder.Called());

    const auto* received =
        recorder.As<SampleCapability>();

    ASSERT_NE(
        received,
        nullptr);

    EXPECT_EQ(
        received->id,
        123);
}

TEST(
    PublisherWorkerTest,
    UnregisteredCapabilityIsIgnored)
{
    // 配信器が登録されていない id が流れてきても落ちないこと。
    Fixture f;

    rim::CapabilityChangeList changes{};

    changes.Add(
        kSlotB);

    f.queue.Push(
        changes);

    EXPECT_TRUE(
        f.worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    PublishesEveryChangedCapability)
{
    Fixture f;

    int aCalls = 0;
    int bCalls = 0;

    rim::GenericCapabilityPublisher publisherA(
        rim::CountingPublish,
        &aCalls);

    rim::GenericCapabilityPublisher publisherB(
        rim::CountingPublish,
        &bCalls);

    f.publisherRegistry.Register(kSlotA, &publisherA);
    f.publisherRegistry.Register(kSlotB, &publisherB);

    rim::CapabilityChangeList changes{};

    changes.Add(kSlotA);
    changes.Add(kSlotB);

    f.queue.Push(
        changes);

    ASSERT_TRUE(
        f.worker.ExecuteOnce());

    EXPECT_EQ(aCalls, 1);
    EXPECT_EQ(bCalls, 1);
}
