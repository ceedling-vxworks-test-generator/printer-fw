#include <gtest/gtest.h>

#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

#include "MachineCapabilityStore.hpp"
#include "ErrorInfo.hpp"

TEST(
    PublisherWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::PublisherInputQueue queue;

    rim::MachineCapabilityStore store;


    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;
    
    rim::PeriodicNotifyManager
    periodicNotifyManager;

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

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

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
    periodicNotifyManager;

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

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
    PublishJob)
{
    rim::MachineCapabilityStore
        store;

    rim::SubscriberMailbox
        mailbox;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    bool called = false;

    const auto id =
        subscriptionStore.
            CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            EXPECT_EQ(
                message.target.type,
                rim::NotificationTargetType::Capability);

            EXPECT_EQ(
                message.target.id,
                static_cast<std::uint32_t>(
                    RI_CAPABILITY_JOB));

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_JOB)
    };

    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore);

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

    queue.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_JOB)
        },
        rim::EventPriority::Normal
    });

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

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    bool called = false;

    const auto id =
        subscriptionStore.
            CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            EXPECT_EQ(
                message.target.type,
                rim::NotificationTargetType::Capability);

            EXPECT_EQ(
                message.target.id,
                static_cast<std::uint32_t>(
                    RI_CAPABILITY_ERROR));

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ERROR)
    };
    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore);

    rim::PublisherInputQueue
        queue;

    rim::PublisherWorker
        worker(
            queue,
            publishManager);

    queue.Push(
    {
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ERROR)
        },
        rim::EventPriority::High
    });

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_TRUE(
        called);
}

TEST(
    PublisherWorkerTest,
    RunAndStop)
{
    rim::PublisherInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;
    
    rim::PeriodicNotifyManager
    periodicNotifyManager;

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

    rim::PublisherWorker worker(
        queue,
        publishManager);

    worker.Run();

    worker.Stop();

    SUCCEED();
}