#include <gtest/gtest.h>
#include <atomic>

#include "printer_a.h"

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

#include "test/support/TestWaitHelper.hpp"
#include "PrinterAProductDefinition.hpp"

// #include "ErrorInfo.hpp"

TEST(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::PublisherInputQueue queue;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
    rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            routeProvider);

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

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
    rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            routeProvider);

    rim::PublisherWorker worker(
        queue,
        publishManager);

    queue.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_JOB)
        },
        rim::EventPriority::Normal
    });

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    PublisherWorkerTest,
    RunAndStop)
{
    rim::PublisherInputQueue queue;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::CallbackQueue callbackQueue; 

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
    rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            routeProvider);
    rim::PublisherWorker worker(
        queue,
        publishManager);

    worker.Run();

    worker.Stop();

    SUCCEED();
}