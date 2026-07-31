#include <gtest/gtest.h>

#include "CapabilityChangeSet.hpp"

#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriptionRegistry.hpp"
#include "SubscriberMailbox.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

#include "MachineCapabilityStore.hpp"

TEST(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::PublisherInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::SubscriptionRegistry
        subscriptionRegistry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::PublishManager
        publishManager(
            publisherRegistry);

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

    rim::SubscriptionRegistry
        subscriptionRegistry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::PublisherWorker worker(
        queue,
        publishManager);

    rim::CapabilityChangeSet changes{};

    queue.Push(
        changes);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    PublishJob)
{
    rim::MachineCapabilityStore
        store;

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.SubscribeJob(
        [&](rim::SubscriptionId,
            const rim::JobCapability&)
        {
            called = true;
        });

    rim::ChangeNotifyManager
        notifyManager(
            mailbox,
            callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    publisherRegistry.Register(
        "Job",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    store.GetJob());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::PublisherInputQueue
        queue;

    rim::PublisherWorker
        worker(
            queue,
            publishManager);

    rim::JobCapability cap{};

    cap.jobActive = true;
    cap.jobId = 123;

    store.Store(
        cap);

    rim::CapabilityChangeSet changes{};

    changes.changedCapabilities.push_back(
        "Job");

    queue.Push(
        changes);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_TRUE(
        called);
}

TEST(
    PublisherWorkerTest,
    PublishError)
{
    rim::MachineCapabilityStore
        store;

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.SubscribeError(
        [&](rim::SubscriptionId,
            const rim::ErrorCapability&)
        {
            called = true;
        });

    rim::ChangeNotifyManager
        notifyManager(
            mailbox,
            callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    publisherRegistry.Register(
        "Error",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    store.GetError());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::PublisherInputQueue
        queue;

    rim::PublisherWorker
        worker(
            queue,
            publishManager);

    rim::ErrorCapability
        capability{};

    capability.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    store.Store(
        capability);

    rim::CapabilityChangeSet changes{};

    changes.changedCapabilities.push_back(
        "Error");

    queue.Push(
        changes);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_TRUE(
        called);
}