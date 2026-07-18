#include <gtest/gtest.h>

#include "datastore/DataStoreWorker.hpp"
#include "datastore/AggregateRIMSnapshotReader.hpp"

#include "capability/CapabilityInputQueue.hpp"

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