#include <gtest/gtest.h>

#include <cstdint>

#include "CallbackSubscriptionRegistry.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"

//
// 変化した Capability を「ポーリング購読者(Mailbox)」と
// 「コールバック購読者」の両方へ届ける段。
//
// 旧実装は Capability ごとに Notify のオーバーロードを持ち、しかも
// Consumable / Job だけ Mailbox に積まれない非対称があった(仕様ではなく実装都合)。
// 現在は全 Capability が同じ経路を通る。ここではその対称性も確認する。
//

namespace
{

struct SampleCapability
{
    double value{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

rim::CapabilityPayload Make(
    double value)
{
    SampleCapability cap{};

    cap.value = value;

    return rim::CapabilityPayload::From(
        cap);
}

}

TEST(
    ChangeNotifyManagerTest,
    NotifyPushesToMailbox)
{
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager manager(
        mailbox,
        callbackRegistry);

    manager.Notify(
        kSlotA,
        Make(300.15));

    rim::CapabilityPayload output;

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotA,
            output));

    const auto* cap =
        output.As<SampleCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_DOUBLE_EQ(
        cap->value,
        300.15);
}

TEST(
    ChangeNotifyManagerTest,
    NotifyInvokesCallback)
{
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    double received = 0.0;

    callbackRegistry.Subscribe(
        kSlotA,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload& payload)
        {
            called = true;

            const auto* cap =
                payload.As<SampleCapability>();

            if (cap != nullptr)
            {
                received = cap->value;
            }
        });

    rim::ChangeNotifyManager manager(
        mailbox,
        callbackRegistry);

    manager.Notify(
        kSlotA,
        Make(42.0));

    EXPECT_TRUE(
        called);

    EXPECT_DOUBLE_EQ(
        received,
        42.0);
}

TEST(
    ChangeNotifyManagerTest,
    CallbackForAnotherCapabilityIsNotInvoked)
{
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.Subscribe(
        kSlotB,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            called = true;
        });

    rim::ChangeNotifyManager manager(
        mailbox,
        callbackRegistry);

    manager.Notify(
        kSlotA,
        Make(1.0));

    EXPECT_FALSE(
        called);
}

TEST(
    ChangeNotifyManagerTest,
    AllCapabilitiesReachBothPaths)
{
    // どの Capability でも Mailbox とコールバックの両方に届くこと
    // (旧実装にあった種別ごとの非対称が無いこと)。
    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    int callbackCalls = 0;

    for (rim::CapabilityId id = 0; id < 2; ++id)
    {
        callbackRegistry.Subscribe(
            id,
            [&](rim::SubscriptionId,
                rim::CapabilityId,
                const rim::CapabilityPayload&)
            {
                ++callbackCalls;
            });
    }

    rim::ChangeNotifyManager manager(
        mailbox,
        callbackRegistry);

    manager.Notify(kSlotA, Make(1.0));
    manager.Notify(kSlotB, Make(2.0));

    EXPECT_EQ(
        callbackCalls,
        2);

    EXPECT_EQ(
        mailbox.Count(kSlotA),
        1U);

    EXPECT_EQ(
        mailbox.Count(kSlotB),
        1U);
}
