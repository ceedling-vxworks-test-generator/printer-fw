#include <gtest/gtest.h>

#include "ChangeNotifyManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "DeliveryMethod.hpp"
#include "NotificationTrigger.hpp"
#include "rim_capability_id.h"
#include "NotificationTargetType.hpp"


TEST(
    ChangeNotifyManagerTest,
    NotifyErrorToMailbox)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

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
        callbackRegistry);

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
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

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

    store.Register(
        info);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ERROR)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyPrintReadyCallback)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

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
                    RI_CAPABILITY_PRINT_READY));

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id = id;
    info.target =
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_PRINT_READY)
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
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_PRINT_READY)
    },
    rim::NotificationTrigger::OnChange);
    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyConsumableCallback)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

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
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_PRINT_READY)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyJobCallback)
{

    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

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
        static_cast<std::uint32_t>(RI_CAPABILITY_JOB)
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
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_JOB)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyEnvironmentCallback)
{

    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

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
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    IgnorePeriodicSubscriberForOnChangeNotification)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

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
        callbackRegistry);

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
        callbackRegistry);

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

    bool called = false;

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
        callbackRegistry);

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(RI_CAPABILITY_ENVIRONMENT)
    },
    rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    IgnorePeriodicMailboxSubscriberForOnChangeNotification)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager mailboxManager;

    rim::CallbackSubscriptionRegistry callbackRegistry;

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
        callbackRegistry);

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