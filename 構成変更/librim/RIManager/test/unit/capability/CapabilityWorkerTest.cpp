#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityWorker.hpp"

#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"

#include "PublisherInputQueue.hpp"

TEST(
    CapabilityWorkerTest,
    ExecuteOnce)
{
    rim::CapabilityInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::ErrorRepository errorRepository;

    rim::CapabilityManager manager(
        store,
        errorRepository);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        store,
        publisherQueue);

    rim::RIMSnapshot snapshot{};

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        300.15);

    rim::AddDoubleItem(
        snapshot,
        rim::RIMDataId::kHumiditySensor,
        60.0);

    queue.Push(
        snapshot);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::CapabilityInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::ErrorRepository errorRepository;

    rim::CapabilityManager manager(
        store,
        errorRepository);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
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

    rim::PublisherInputQueue publisherQueue;

    rim::MachineCapabilityStore store;

    rim::ErrorRepository errorRepository;

    rim::CapabilityManager manager(
        store,
        errorRepository);

    rim::CapabilityWorker worker(
        queue,
        manager,
        store,
        publisherQueue);

    rim::RIMSnapshot snapshot{};

    queue.Push(
        snapshot);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityChangeSet changes{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            changes));

    EXPECT_FALSE(
        changes.changedCapabilities.empty());

}