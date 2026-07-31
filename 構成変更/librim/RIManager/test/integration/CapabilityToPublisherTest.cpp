#include <gtest/gtest.h>

#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityPublisherRegistry.hpp"
#include "CapabilityStore.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"
#include "PublishManager.hpp"
#include "SubscriberMailbox.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"

//
// Capability 保持 -> 配信 -> Mailbox、のつなぎ。
//
// 旧試験は "Environment" という文字列で配信器を登録していた。
// 現在は CapabilityId(整数)で引くので、綴り間違いが起きようがない。
//

TEST(
    CapabilityToPublisherTest,
    NotifyEnvironmentChanged)
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

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        mailbox.Pop(
            rim::kCapEnvironment,
            payload));

    const auto* out =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        out,
        nullptr);

    EXPECT_DOUBLE_EQ(
        out->temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        out->humidity,
        60.0);
}

TEST(
    CapabilityToPublisherTest,
    PublishUnknownCapabilityIsIgnored)
{
    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::PublishManager publishManager(
        publisherRegistry);

    // 配信器が無い id を叩いても落ちない
    publishManager.Publish(
        rim::kCapJob);

    SUCCEED();
}
