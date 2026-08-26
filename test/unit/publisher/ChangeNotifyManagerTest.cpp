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
#include "test/support/NotificationTestHelper.hpp"

class ChangeNotifyManagerTest
    : public ::testing::Test
{
protected:
    rim::SubscriptionStore store;
    rim::SubscriberMailboxManager mailboxManager;
    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::CallbackQueue callbackQueue;
    rim::ChangeNotifyManager manager
        {
            store,
            mailboxManager,
            callbackRegistry,
            callbackQueue
        };

    rim::SubscriptionId RegisterSubscription(
        std::uint32_t capabilityId,
        rim::DeliveryMethod method,
        rim::NotificationTrigger trigger)
    {
        const auto id = store.CreateSubscriptionId();

        store.Register(
        {
            id,
            test::CapabilityTarget(capabilityId),
            method,
            trigger
        });

        return id;
    }
};

class CallbackNotificationTest
    : public ChangeNotifyManagerTest,
      public ::testing::WithParamInterface<
            std::uint32_t>
{
};


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

class RecordingCustomDeliveryHandler
    : public rim::ICustomDeliveryHandler
{
public:

    bool Deliver(
        const rim::SubscriptionInfo& info,
        const rim::NotificationMessage& message)
        override
    {
        called = true;

        receivedInfo = info;
        receivedMessage = message;

        return true;
    }

    bool called{false};

    rim::SubscriptionInfo
        receivedInfo{};

    rim::NotificationMessage
        receivedMessage{};
};

class FakeEventSender
{
public:

    void Send(
        const rim::NotificationMessage& message)
    {
        called = true;
        lastMessage = message;
    }

    bool called{false};

    rim::NotificationMessage
        lastMessage{};
};

class EventForwardingHandler
    : public rim::ICustomDeliveryHandler
{
public:

    explicit EventForwardingHandler(
        FakeEventSender& sender)
        : sender_(sender)
    {
    }

    bool Deliver(
        const rim::SubscriptionInfo&,
        const rim::NotificationMessage& message)
        override
    {
        sender_.Send(message);

        return true;
    }

private:

    FakeEventSender& sender_;
};


TEST_F(
    ChangeNotifyManagerTest,
    NotifyErrorToMailbox)
{

    const auto id =
        RegisterSubscription(
            RI_CAPABILITY_ERROR,
            rim::DeliveryMethod::Mailbox,
            rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ERROR),
        rim::NotificationTrigger::OnChange
    );

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

TEST_F(
    ChangeNotifyManagerTest,
    IgnorePeriodicSubscriberForOnChangeNotification)
{

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
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::Periodic;

    store.Register(
        info);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_FALSE(
        called);
}

TEST_F(
    ChangeNotifyManagerTest,
    IgnoreOnChangeSubscriberForPeriodicNotification)
{

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
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    info.method =
        rim::DeliveryMethod::Callback;
    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    manager.Notify(
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::NotificationTrigger::Periodic);
    EXPECT_FALSE(
        called);
}

TEST_F(
    ChangeNotifyManagerTest,
    NotifyMatchingTriggerSubscriber)
{

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
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(
        info);

    callbackWorker.Run();

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
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


TEST_F(
    ChangeNotifyManagerTest,
    IgnorePeriodicCallbackSubscriberForOnChangeNotification)
{

    store.Register(
    {
        100,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::Periodic
    });

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    auto& subscriberMailbox =
        mailboxManager.GetMailbox(
            100);

   rim::NotificationMessage message{};

    EXPECT_FALSE(
        subscriberMailbox.Pop(
            message));
}

TEST_F(
    ChangeNotifyManagerTest,
    IgnoreDifferentTarget)
{
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
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    store.Register(info);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_JOB),
        rim::NotificationTrigger::OnChange);

    EXPECT_FALSE(
        called);
}
TEST_F(
    ChangeNotifyManagerTest,
    NotifyAllMatchingSubscribers)
{
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
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    store.Register(
    {
        id2,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    callbackWorker.Run();

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
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

TEST_F(
    ChangeNotifyManagerTest,
    CustomDeliveryWithoutHandler)
{
    const auto id =
        RegisterSubscription(
            RI_CAPABILITY_ENVIRONMENT,
            rim::DeliveryMethod::Custom,
            rim::NotificationTrigger::OnChange);

    EXPECT_NO_THROW(
        manager.Notify(
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::NotificationTrigger::OnChange));

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        0U);
}

TEST_F(
    ChangeNotifyManagerTest,
    CustomDeliverySucceeded)
{

    SuccessfulCustomDeliveryHandler handler;

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
        RegisterSubscription(
            RI_CAPABILITY_ENVIRONMENT,
            rim::DeliveryMethod::Custom,
            rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        handler.called_);

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        0U);
}

TEST_F(
    ChangeNotifyManagerTest,
    CustomDeliveryFailedCountAccumulates)
{

    FailingCustomDeliveryHandler handler;

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
    RegisterSubscription(
        RI_CAPABILITY_ENVIRONMENT,
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_EQ(
        manager.GetCustomDeliveryFailureCount(),
        3U);
}

TEST_F(
    ChangeNotifyManagerTest,
    CustomDeliveryReceivesSubscriptionAndMessage)
{

    RecordingCustomDeliveryHandler handler;

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
    RegisterSubscription(
        RI_CAPABILITY_ENVIRONMENT,
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        handler.called);

    EXPECT_EQ(
        handler.receivedInfo.id,
        id);

    EXPECT_EQ(
        handler.receivedMessage.target.type,
        rim::NotificationTargetType::Capability);

    EXPECT_EQ(
        handler.receivedMessage.target.id,
        RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        handler.receivedMessage.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST_F(
    ChangeNotifyManagerTest,
    CustomDeliveryCanForwardNotificationToEventSender)
{

    FakeEventSender
        sender;

    EventForwardingHandler
        handler(sender);

    manager.SetCustomDeliveryHandler(
        &handler);

    const auto id =
    RegisterSubscription(
        RI_CAPABILITY_ENVIRONMENT,
        rim::DeliveryMethod::Custom,
        rim::NotificationTrigger::OnChange);

    manager.Notify(
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::NotificationTrigger::OnChange);

    EXPECT_TRUE(
        sender.called);

    EXPECT_EQ(
        sender.lastMessage.target.id,
        RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        sender.lastMessage.trigger,
        rim::NotificationTrigger::OnChange);
}

TEST_P(
    CallbackNotificationTest,
    NotifyCallback)
{
    rim::CallbackWorker
        callbackWorker(
            callbackQueue,
            callbackRegistry);

    std::atomic<bool>
        called{false};

    const auto capabilityId =
        GetParam();

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
                capabilityId);

            called = true;
        });

    store.Register(
    {
        id,
        test::CapabilityTarget(capabilityId),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    callbackWorker.Run();

    manager.Notify(
        test::CapabilityTarget(capabilityId),
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

INSTANTIATE_TEST_SUITE_P(
    CallbackNotifications,
    CallbackNotificationTest,
    ::testing::Values(
        static_cast<std::uint32_t>(RI_CAPABILITY_ERROR),
        static_cast<std::uint32_t>(RI_CAPABILITY_PRINT_READY),
        static_cast<std::uint32_t>(RI_CAPABILITY_JOB)));
