#include <gtest/gtest.h>

#include "capability/CapabilityWorker.hpp"

#include "capability/CapabilityManager.hpp"
#include "capability/MachineCapabilityStore.hpp"

#include "capability/PrinterACapabilityRuleSet.hpp"

#include "publisher/PublisherInputQueue.hpp"

TEST(
    CapabilityWorkerTest,
    ExecuteOnce)
{
    rim::CapabilityInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::PrinterACapabilityRuleSet rules;

    rim::CapabilityManager manager(
        store,
        rules);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue);

    rim::RIMSnapshot snapshot{};

    snapshot.temperature = 300.15;
    snapshot.humidity = 60.0;

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

    rim::PrinterACapabilityRuleSet rules;

    rim::CapabilityManager manager(
        store,
        rules);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
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

    rim::PrinterACapabilityRuleSet rules;

    rim::CapabilityManager manager(
        store,
        rules);

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue);

    rim::RIMSnapshot snapshot{};

    queue.Push(
        snapshot);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::PublisherInput input{};

    EXPECT_TRUE(
        publisherQueue.TryPop(
            input));
}