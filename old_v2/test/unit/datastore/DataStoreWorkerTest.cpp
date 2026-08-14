#include <gtest/gtest.h>

#include "DataStoreWorker.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "ErrorRepository.hpp"
#include "FaultApplier.hpp"

#include "CapabilityInputQueue.hpp"

TEST(
    DataStoreWorkerTest,
    ExecuteOnce)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        queue,
        store,
        reader,
        capabilityQueue);

    rim::RIMDataItem item{};

    queue.Push(
        item);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    DataStoreWorkerTest,
    GenerateSnapshotEvent)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        queue,
        store,
        reader,
        capabilityQueue);

    rim::RIMDataItem item{};

    queue.Push(
        item);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::RIMSnapshot snapshot{};

    EXPECT_TRUE(
        capabilityQueue.TryPop(
            snapshot));
}

//
// faultSnapshotQueue - FaultInfoList 経路(併存する別レーン)を同じワーカが処理すること。
//

TEST(
    DataStoreWorkerTest,
    ExecuteOnceDrainsFaultSnapshotQueue)
{
    rim::StoreInputQueue queue;
    rim::ValueStore store;
    rim::AggregateRIMSnapshotReader reader(store);
    rim::CapabilityInputQueue capabilityQueue;

    rim::ErrorRepository repository;
    rim::FaultApplier    faultApplier(repository);

    rim::FaultSnapshotQueue faultSnapshotQueue;

    rim::DataStoreWorker worker(
        queue,
        store,
        reader,
        capabilityQueue,
        &faultApplier,
        &faultSnapshotQueue);

    rim::FaultSnapshotBatch batch{};
    batch.entries[0] = {1001, rim::ErrorState::kActive};
    batch.count      = 1;

    faultSnapshotQueue.Push(
        batch);

    EXPECT_TRUE(
        worker.ExecuteOnce());

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);
    EXPECT_EQ(
        repository.GetAll()[0].errorCode,
        1001U);

    // 通常レーンと同じく、反映後は Capability 再生成のための Snapshot を送る。
    rim::RIMSnapshot snapshot{};
    EXPECT_TRUE(
        capabilityQueue.TryPop(
            snapshot));
}

TEST(
    DataStoreWorkerTest,
    ExecuteOncePrefersFaultSnapshotQueueWhenBothHaveItems)
{
    rim::StoreInputQueue queue;
    rim::ValueStore store;
    rim::AggregateRIMSnapshotReader reader(store);
    rim::CapabilityInputQueue capabilityQueue;

    rim::ErrorRepository repository;
    rim::FaultApplier    faultApplier(repository);

    rim::FaultSnapshotQueue faultSnapshotQueue;

    rim::DataStoreWorker worker(
        queue,
        store,
        reader,
        capabilityQueue,
        &faultApplier,
        &faultSnapshotQueue);

    rim::RIMDataItem item{};
    queue.Push(
        item);

    rim::FaultSnapshotBatch batch{};
    batch.entries[0] = {2002, rim::ErrorState::kRecovered};
    batch.count      = 1;
    faultSnapshotQueue.Push(
        batch);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    // スナップショット側が先に消費され、通常レーンはまだ残っている。
    EXPECT_TRUE(
        repository.GetAll().Size() == 1U);
    EXPECT_EQ(
        queue.Size(),
        1U);
}