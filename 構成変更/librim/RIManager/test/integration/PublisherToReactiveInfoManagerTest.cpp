#include <gtest/gtest.h>

#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityPublisherRegistry.hpp"
#include "CapabilityStore.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"
#include "NotificationReceiver.hpp"
#include "PublishManager.hpp"
#include "SubscriberMailbox.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

//
// 配信 -> 購読者の受け取りまで。
//

TEST(
    PublisherToReactiveInfoManagerTest,
    ReceiveEnvironmentNotification)
{
    rim::CapabilityStore store;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::GenericCapabilityPublisher publisher(
        [&]
        {
            rim::CapabilityPayload payload;

            if (store.TryGet(
                    rim::kCapEnvironment,
                    payload))
            {
                notifyManager.Notify(
                    rim::kCapEnvironment,
                    payload);
            }
        });

    publisherRegistry.Register(
        rim::kCapEnvironment,
        &publisher);

    rim::PublishManager publishManager(
        publisherRegistry);

    rim::EnvironmentCapability capability{};

    capability.temperature = 300.15;
    capability.humidity    = 60.0;

    store.Store(
        rim::kCapEnvironment,
        rim::CapabilityPayload::From(
            capability));

    publishManager.Publish(
        rim::kCapEnvironment);

    rim::NotificationReceiver receiver(
        mailbox);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        receiver.TryGet(
            rim::kCapEnvironment,
            payload));

    const auto* received =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        received,
        nullptr);

    EXPECT_DOUBLE_EQ(
        received->temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        received->humidity,
        60.0);
}

TEST(
    PublisherToReactiveInfoManagerTest,
    CallbackSubscriberAlsoReceivesIt)
{
    // ポーリング購読とコールバック購読の両方に届くこと。
    rim::CapabilityStore store;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.Subscribe(
        rim::kCapEnvironment,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            called = true;
        });

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::EnvironmentCapability capability{};

    store.Store(
        rim::kCapEnvironment,
        rim::CapabilityPayload::From(
            capability));

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        store.TryGet(
            rim::kCapEnvironment,
            payload));

    notifyManager.Notify(
        rim::kCapEnvironment,
        payload);

    EXPECT_TRUE(
        called);

    rim::NotificationReceiver receiver(
        mailbox);

    EXPECT_TRUE(
        receiver.HasPending(
            rim::kCapEnvironment));
}
