#include <gtest/gtest.h>

#include <cstdint>

#include "CallbackSubscriptionRegistry.hpp"

//
// コールバック購読。
//
// 旧実装は SubscribeEnvironment / SubscribeError / … と Capability ごとに
// 購読関数と std::vector を持っていた(Capability を増やすたびに Core を改修)。
// 現在は購読したい CapabilityId を引数で渡す1本にまとまっている。
//

namespace
{

struct SampleCapability
{
    std::int32_t value{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

rim::CapabilityPayload Make(
    std::int32_t value)
{
    SampleCapability cap{};

    cap.value = value;

    return rim::CapabilityPayload::From(
        cap);
}

}

TEST(
    CallbackSubscriptionRegistryTest,
    SubscribeAndNotify)
{
    rim::CallbackSubscriptionRegistry registry;

    bool called = false;

    const auto id =
        registry.Subscribe(
            kSlotA,
            [&](rim::SubscriptionId,
                rim::CapabilityId,
                const rim::CapabilityPayload&)
            {
                called = true;
            });

    EXPECT_NE(
        id,
        rim::kInvalidSubscriptionId);

    EXPECT_EQ(
        registry.Notify(
            kSlotA,
            Make(1)),
        1U);

    EXPECT_TRUE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    CallbackReceivesIdAndPayload)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::SubscriptionId receivedSubscription =
        rim::kInvalidSubscriptionId;

    rim::CapabilityId receivedCapability =
        rim::kInvalidCapabilityId;

    std::int32_t receivedValue = 0;

    const auto id =
        registry.Subscribe(
            kSlotA,
            [&](rim::SubscriptionId subscription,
                rim::CapabilityId capability,
                const rim::CapabilityPayload& payload)
            {
                receivedSubscription = subscription;
                receivedCapability   = capability;

                const auto* cap =
                    payload.As<SampleCapability>();

                if (cap != nullptr)
                {
                    receivedValue = cap->value;
                }
            });

    registry.Notify(
        kSlotA,
        Make(7));

    EXPECT_EQ(
        receivedSubscription,
        id);

    EXPECT_EQ(
        receivedCapability,
        kSlotA);

    EXPECT_EQ(
        receivedValue,
        7);
}

TEST(
    CallbackSubscriptionRegistryTest,
    OnlyMatchingCapabilityIsNotified)
{
    rim::CallbackSubscriptionRegistry registry;

    int aCalls = 0;
    int bCalls = 0;

    registry.Subscribe(
        kSlotA,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            ++aCalls;
        });

    registry.Subscribe(
        kSlotB,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            ++bCalls;
        });

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_EQ(aCalls, 1);
    EXPECT_EQ(bCalls, 0);
}

TEST(
    CallbackSubscriptionRegistryTest,
    MultipleSubscribersOnSameCapability)
{
    rim::CallbackSubscriptionRegistry registry;

    int calls = 0;

    for (int i = 0; i < 3; ++i)
    {
        registry.Subscribe(
            kSlotA,
            [&](rim::SubscriptionId,
                rim::CapabilityId,
                const rim::CapabilityPayload&)
            {
                ++calls;
            });
    }

    EXPECT_EQ(
        registry.Notify(
            kSlotA,
            Make(1)),
        3U);

    EXPECT_EQ(calls, 3);
}

TEST(
    CallbackSubscriptionRegistryTest,
    Unsubscribe)
{
    rim::CallbackSubscriptionRegistry registry;

    bool called = false;

    const auto id =
        registry.Subscribe(
            kSlotA,
            [&](rim::SubscriptionId,
                rim::CapabilityId,
                const rim::CapabilityPayload&)
            {
                called = true;
            });

    EXPECT_TRUE(
        registry.Unsubscribe(
            id));

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_FALSE(
        called);
}

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeUnknownIdReturnsFalse)
{
    rim::CallbackSubscriptionRegistry registry;

    EXPECT_FALSE(
        registry.Unsubscribe(
            9999));
}

TEST(
    CallbackSubscriptionRegistryTest,
    UnsubscribeKeepsOtherSubscribers)
{
    // 内部で末尾を詰めて削除するため、残りが壊れないことを確認する。
    rim::CallbackSubscriptionRegistry registry;

    int remainingCalls = 0;

    const auto first =
        registry.Subscribe(
            kSlotA,
            [&](rim::SubscriptionId,
                rim::CapabilityId,
                const rim::CapabilityPayload&)
            {
            });

    registry.Subscribe(
        kSlotA,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            ++remainingCalls;
        });

    ASSERT_TRUE(
        registry.Unsubscribe(
            first));

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_EQ(
        remainingCalls,
        1);

    EXPECT_EQ(
        registry.Count(),
        1U);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsEmptyCallback)
{
    rim::CallbackSubscriptionRegistry registry;

    EXPECT_EQ(
        registry.Subscribe(
            kSlotA,
            rim::CapabilityCallback{}),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsOutOfRangeCapabilityId)
{
    rim::CallbackSubscriptionRegistry registry;

    const rim::CapabilityId outOfRange =
        static_cast<rim::CapabilityId>(
            rim::kCapabilityMaxCount);

    EXPECT_EQ(
        registry.Subscribe(
            outOfRange,
            [](rim::SubscriptionId,
               rim::CapabilityId,
               const rim::CapabilityPayload&)
            {
            }),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsSubscriptionWhenFull)
{
    // 固定容量なので、満杯になったら採番失敗を返す(黙って伸びない)。
    rim::CallbackSubscriptionRegistry registry;

    for (std::size_t i = 0;
         i < rim::kCapabilitySubscriptionMaxCount;
         ++i)
    {
        ASSERT_NE(
            registry.Subscribe(
                kSlotA,
                [](rim::SubscriptionId,
                   rim::CapabilityId,
                   const rim::CapabilityPayload&)
                {
                }),
            rim::kInvalidSubscriptionId);
    }

    EXPECT_EQ(
        registry.Subscribe(
            kSlotA,
            [](rim::SubscriptionId,
               rim::CapabilityId,
               const rim::CapabilityPayload&)
            {
            }),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    CountsSubscribersPerCapability)
{
    rim::CallbackSubscriptionRegistry registry;

    registry.Subscribe(
        kSlotA,
        [](rim::SubscriptionId,
           rim::CapabilityId,
           const rim::CapabilityPayload&)
        {
        });

    registry.Subscribe(
        kSlotB,
        [](rim::SubscriptionId,
           rim::CapabilityId,
           const rim::CapabilityPayload&)
        {
        });

    registry.Subscribe(
        kSlotB,
        [](rim::SubscriptionId,
           rim::CapabilityId,
           const rim::CapabilityPayload&)
        {
        });

    EXPECT_EQ(
        registry.CountFor(kSlotA),
        1U);

    EXPECT_EQ(
        registry.CountFor(kSlotB),
        2U);
}
