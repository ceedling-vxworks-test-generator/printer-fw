#include <gtest/gtest.h>
#include <atomic>

#include "rim_api.h"

#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriberMailbox.hpp"

#include "NotificationMessage.hpp"

#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"

#include "test/core/support/TestWaitHelper.hpp"
#include "Product.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

class PublisherWorkerTest
    : public ::testing::Test
{
protected:

    rim::PublisherInputQueue queue;

    rim::SubscriptionStore subscriptionStore;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    rim::PeriodicNotifyManager periodicNotifyManager;

    rim::CallbackQueue callbackQueue;

    rim::ChangeNotifyManager notifyManager
    {
        subscriptionStore,
        mailboxManager,
        callbackRegistry,
        callbackQueue
    };

    rim::ProductContext
    productContext
    {
        rim::kProductDefinition
    };

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };
};

TEST_F(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::PublisherWorker worker(
        queue,
        publishManager);

    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST_F(
    PublisherWorkerTest,
    ExecuteOnce)
{
    rim::PublisherWorker worker(
        queue,
        publishManager);

    queue.Push(
    {
        test::CapabilityTarget(RI_CAPABILITY_JOB),
        rim::EventPriority::Normal
    });

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST_F(
    PublisherWorkerTest,
    RunAndStop)
{
    rim::PublisherWorker worker(
        queue,
        publishManager);

    worker.Run();

    worker.Stop();

    SUCCEED();
}