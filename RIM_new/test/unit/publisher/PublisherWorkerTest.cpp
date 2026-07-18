#include <gtest/gtest.h>

#include "publisher/PublisherWorker.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"
#include "publisher/SubscriptionRegistry.hpp"
#include "publisher/SubscriberMailbox.hpp"

#include "capability/MachineCapabilityStore.hpp"

TEST(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::PublisherInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::SubscriptionRegistry registry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        registry,
        mailbox,
        callbackRegistry);

    rim::PublishManager publishManager(
        store,
        notifyManager);

    rim::PublisherWorker worker(
        queue,
        publishManager);

    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    ExecuteOnce)
{
    rim::PublisherInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::SubscriptionRegistry registry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        registry,
        mailbox,
        callbackRegistry);

    rim::PublishManager publishManager(
        store,
        notifyManager);

    rim::PublisherWorker worker(
        queue,
        publishManager);

    queue.Push(
        rim::PublisherInput{});

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

