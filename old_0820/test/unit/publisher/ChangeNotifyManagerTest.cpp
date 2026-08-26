#include <gtest/gtest.h>
#include <atomic>

#include "printer_a.h"

#include "ChangeNotifyManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "DeliveryMethod.hpp"
#include "NotificationTrigger.hpp"
#include "NotificationTargetType.hpp"
#include "ICustomDeliveryHandler.hpp"
#include "CallbackWorker.hpp"
#include "CallbackQueue.hpp"

#include "test/support/TestWaitHelper.hpp"

namespace
{

    class FailingCustomDeliveryHandler
        : public rim::ICustomDeliveryHandler
    {
    public:

        bool Deliver(
            const rim::SubscriptionInfo&,
            const rim::NotificationMessage&)
            override
        {
            //MessageSend,EventSend�Ȃǒǉ��������z�M���@������
            return false;
        }
    };

}

class SuccessfulCustomDeliveryHandler
    : public rim::ICustomDeliveryHandler
{
public:

    bool Deliver(
        const rim::SubscriptionInfo&,
        const rim::NotificationMessage&)
        override
    {
        called_ = true;
        return true;
    }

    bool called_{false};
};


TEST(
    ChangeNotifyManagerTest,
    NotifyErrorToMailbox)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    const auto id =
        store.CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ERROR)
    };

    info.method =
        rim::DeliveryMethod::Mailbox;
    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry
        ,callbackQueue);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ERROR)
    },
    rim::NotificationTrigger::OnChange);

    rim::NotificationMessage message{};

    ASSERT_TRUE(
        mailboxManager
            .GetMailbox(id)
            .Pop(message));

    EXPECT_EQ(
    message.target.type,
    rim::NotificationTargetType::Capability);

    EXPECT_EQ(
    message.target.id,
    static_cast<uint32_t>(RI_CAPABILITY_ERROR));
    EXPECT_EQ(
        message.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyErrorCallback)
{
    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue
        callbackQueue;

    rim::CallbackWorker
        callbackWorker(
            callbackQueue,
            callbackRegistry);

    std::atomic<bool>
        called{false};

    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        subscriptionId,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id =
        subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ERROR)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::ChangeNotifyManager
        manager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    callbackWorker.Run();

    manager.Notify(
        info.target,
        rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return called.load();
            },
            std::chrono::milliseconds(
                100)));

    callbackWorker.Stop();
}

TEST(
    ChangeNotifyManagerTest,
    NotifyConsumableCallback)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    std::atomic<bool> called{false};

    const auto id =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            EXPECT_EQ(
                subscriptionId,
                id);

            EXPECT_EQ(
                message.target.type,
                rim::NotificationTargetType::Capability);

            EXPECT_EQ(
                message.target.id,
                static_cast<std::uint32_t>(
                    RI_CAPABILITY_PRINT_READY));

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    callbackWorker.Run();

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return called.load();
            },
            std::chrono::milliseconds(100)));

    callbackWorker.Stop();
}


TEST(
    ChangeNotifyManagerTest,
    NotifyJobCallback)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    std::atomic<bool> called{false};

    const auto id =
        store.CreateSubscriptionId();

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
        static_cast<std::uint32_t>(
            RI_CAPABILITY_JOB)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    callbackWorker.Run();

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_JOB)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return called.load();
            },
            std::chrono::milliseconds(100)));

    callbackWorker.Stop();
}

TEST(
    ChangeNotifyManagerTest,
    IgnorePeriodicSubscriberForOnChangeNotification)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::Periodic;

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_FALSE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    IgnoreOnChangeSubscriberForPeriodicNotification)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::Periodic);
    EXPECT_FALSE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyMatchingTriggerSubscriber)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    std::atomic<bool> called{false};

    const auto id =
        store.CreateSubscriptionId();

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
                    RI_CAPABILITY_ENVIRONMENT));

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    callbackWorker.Run();

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return called.load();
            },
            std::chrono::milliseconds(100)));

    callbackWorker.Stop();
}


TEST(
    ChangeNotifyManagerTest,
    IgnorePeriodicMailboxSubscriberForOnChangeNotification)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    store.Register(
    {
        100,
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
        },
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::Periodic
    });

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::OnChange);

    auto& subscriberMailbox =
        mailboxManager.GetMailbox(
            100);

   rim::NotificationMessage message{};

    EXPECT_FALSE(
        subscriberMailbox.Pop(
            message));
}
TEST(
    ChangeNotifyManagerTest,
    IgnoreDifferentTarget)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;
    bool called = false;

    const auto id =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;

    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_JOB)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_FALSE(
        called);
}
TEST(
    ChangeNotifyManagerTest,
    NotifyAllMatchingSubscribers)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    std::atomic<bool> called1{false};
    std::atomic<bool> called2{false};

    const auto id1 =
        store.CreateSubscriptionId();

    const auto id2 =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id1,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called1 = true;
        });

    callbackRegistry.Subscribe(
        id2,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            called2 = true;
        });

    store.Register(
    {
        id1,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    store.Register(
    {
        id2,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    callbackWorker.Run();

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return
                    called1.load()
                    &&
                    called2.load();
            },
            std::chrono::milliseconds(100)));

    callbackWorker.Stop();
}

TEST(
    ChangeNotifyManagerTest,
    CustomDeliveryWithoutHandler)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    const auto id =
        store.CreateSubscriptionId();

    store.Register(
    {
        id,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange
    });

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    EXPECT_NO_THROW(
        manager.Notify(
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::NotificationTrigger::OnChange));

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        0U);
}

TEST(
    ChangeNotifyManagerTest,
    CustomDeliverySucceeded)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    SuccessfulCustomDeliveryHandler handler;

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
        store.CreateSubscriptionId();

    store.Register(
    {
        id,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange
    });

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        handler.called_);

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        0U);
}

TEST(
    ChangeNotifyManagerTest,
    CustomDeliveryFailedCountAccumulates)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;

    FailingCustomDeliveryHandler handler;

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
        store.CreateSubscriptionId();

    store.Register(
    {
        id,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange
    });

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        3U);
}



