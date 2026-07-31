#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

#include "MachineCapabilityStore.hpp"
#include "ErrorInfo.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"
#include "SubscriptionRegistry.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

TEST(
    EndToEndNotificationFlowTest,
    EnvironmentNotification)
{
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

    publisherRegistry.Register(
        "Environment",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    store.GetEnvironment());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    store.Store(
        input);

    publishManager.Publish(
        "Environment");

    rim::NotificationReceiver
        receiver(
            mailbox);

    rim::EnvironmentCapability output{};

    ASSERT_TRUE(
        receiver.TryGetEnvironment(
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output.humidity,
        60.0);
}

TEST(
    EndToEndNotificationFlowTest,
    ErrorNotification)
{
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

    rim::ErrorCapability input{};

    input.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    store.Store(
        input);

    publishManager.Publish(
        "Error");

    rim::NotificationReceiver
        receiver(
            mailbox);

    rim::ErrorCapability output{};

    ASSERT_TRUE(
        receiver.TryGetError(
            output));

    ASSERT_EQ(
        output.errors.size(),
        1U);

    EXPECT_EQ(
        output.errors.front().errorCode,
        1001U);

    EXPECT_EQ(
        output.errors.front().state,
        rim::ErrorState::kActive);

    EXPECT_EQ(
        output.errors.front().severity,
        rim::ErrorSeverity::kError);
}

TEST(
    EndToEndNotificationFlowTest,
    PrintReadyNotification)
{
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

    publisherRegistry.Register(
        "PrintReady",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    store.GetPrintReady());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::PrintReadyCapability input{};

    input.ready = true;

    store.Store(
        input);

    publishManager.Publish(
        "PrintReady");

    rim::NotificationReceiver
        receiver(
            mailbox);

    rim::PrintReadyCapability output{};

    ASSERT_TRUE(
        receiver.TryGetPrintReady(
            output));

    EXPECT_TRUE(
        output.ready);
}