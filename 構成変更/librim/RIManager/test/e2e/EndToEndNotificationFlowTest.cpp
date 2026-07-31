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
// Capability を格納してから購読者が受け取るまでの一連。
//
// 旧試験は Capability ごとにほぼ同じコードを3回書いていた(文字列キーの登録と
// TryGetXxx の呼び分けが型ごとに違ったため)。id 引数方式になったので、
// 組み立てを共通化して中身の型だけを変えれば済むようになった。
//

namespace
{

// 配信段一式。
struct Fixture
{
    rim::CapabilityStore              store;
    rim::SubscriberMailbox            mailbox;
    rim::CallbackSubscriptionRegistry callbackRegistry;
    rim::ChangeNotifyManager          notifyManager{mailbox, callbackRegistry};
    rim::CapabilityPublisherRegistry  publisherRegistry;
    rim::PublishManager               publishManager{publisherRegistry};

    // 指定 id の現在値を通知経路へ流す配信器を登録する。
    void RegisterPublisher(
        rim::CapabilityId id,
        rim::GenericCapabilityPublisher& publisher)
    {
        publisherRegistry.Register(
            id,
            &publisher);
    }
};

}

TEST(
    EndToEndNotificationFlowTest,
    EnvironmentNotification)
{
    Fixture f;

    rim::GenericCapabilityPublisher publisher(
        [&]
        {
            rim::CapabilityPayload payload;

            if (f.store.TryGet(
                    rim::kCapEnvironment,
                    payload))
            {
                f.notifyManager.Notify(
                    rim::kCapEnvironment,
                    payload);
            }
        });

    f.RegisterPublisher(
        rim::kCapEnvironment,
        publisher);

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity    = 60.0;

    f.store.Store(
        rim::kCapEnvironment,
        rim::CapabilityPayload::From(
            input));

    f.publishManager.Publish(
        rim::kCapEnvironment);

    rim::NotificationReceiver receiver(
        f.mailbox);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        receiver.TryGet(
            rim::kCapEnvironment,
            payload));

    const auto* output =
        payload.As<
            rim::EnvironmentCapability>();

    ASSERT_NE(
        output,
        nullptr);

    EXPECT_DOUBLE_EQ(
        output->temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output->humidity,
        60.0);
}

TEST(
    EndToEndNotificationFlowTest,
    ErrorNotification)
{
    Fixture f;

    rim::GenericCapabilityPublisher publisher(
        [&]
        {
            rim::CapabilityPayload payload;

            if (f.store.TryGet(
                    rim::kCapError,
                    payload))
            {
                f.notifyManager.Notify(
                    rim::kCapError,
                    payload);
            }
        });

    f.RegisterPublisher(
        rim::kCapError,
        publisher);

    rim::ErrorCapability input{};

    input.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    f.store.Store(
        rim::kCapError,
        rim::CapabilityPayload::From(
            input));

    f.publishManager.Publish(
        rim::kCapError);

    rim::NotificationReceiver receiver(
        f.mailbox);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        receiver.TryGet(
            rim::kCapError,
            payload));

    const auto* output =
        payload.As<
            rim::ErrorCapability>();

    ASSERT_NE(
        output,
        nullptr);

    ASSERT_EQ(
        output->count,
        1U);

    EXPECT_EQ(
        output->errors[0].errorCode,
        1001U);

    EXPECT_EQ(
        output->errors[0].state,
        rim::ErrorState::kActive);

    EXPECT_EQ(
        output->errors[0].severity,
        rim::ErrorSeverity::kError);
}

TEST(
    EndToEndNotificationFlowTest,
    PrintReadyNotification)
{
    Fixture f;

    rim::GenericCapabilityPublisher publisher(
        [&]
        {
            rim::CapabilityPayload payload;

            if (f.store.TryGet(
                    rim::kCapPrintReady,
                    payload))
            {
                f.notifyManager.Notify(
                    rim::kCapPrintReady,
                    payload);
            }
        });

    f.RegisterPublisher(
        rim::kCapPrintReady,
        publisher);

    rim::PrintReadyCapability input{};

    input.ready = true;

    f.store.Store(
        rim::kCapPrintReady,
        rim::CapabilityPayload::From(
            input));

    f.publishManager.Publish(
        rim::kCapPrintReady);

    rim::NotificationReceiver receiver(
        f.mailbox);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        receiver.TryGet(
            rim::kCapPrintReady,
            payload));

    const auto* output =
        payload.As<
            rim::PrintReadyCapability>();

    ASSERT_NE(
        output,
        nullptr);

    EXPECT_TRUE(
        output->ready);
}
