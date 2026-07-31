#include <gtest/gtest.h>

#include <cstdint>

#include "CallbackSubscriptionRegistry.hpp"

#include "test/support/CallbackTestHelper.hpp"

//
// コールバック購読。
//
// 旧実装は SubscribeEnvironment / SubscribeError / … と Capability ごとに
// 購読関数と std::vector を持っていた(Capability を増やすたびに Core を改修)。
// 現在は購読したい CapabilityId を引数で渡す1本にまとまっている。
//
// コールバックは std::function ではなく **関数ポインタ + userData** である
// (暗黙の動的確保と例外送出を経路から外すため)。捕捉付きラムダは渡せないので、
// 状態は CapabilityRecorder に置いてそのアドレスを userData で渡す。
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

    rim::CapabilityRecorder recorder;

    const auto id =
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &recorder);

    EXPECT_NE(
        id,
        rim::kInvalidSubscriptionId);

    EXPECT_EQ(
        registry.Notify(
            kSlotA,
            Make(1)),
        1U);

    EXPECT_TRUE(
        recorder.Called());
}

TEST(
    CallbackSubscriptionRegistryTest,
    CallbackReceivesIdAndPayload)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    const auto id =
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &recorder);

    registry.Notify(
        kSlotA,
        Make(7));

    EXPECT_EQ(
        recorder.lastSubscription.load(),
        id);

    EXPECT_EQ(
        recorder.lastCapability.load(),
        kSlotA);

    const auto* cap =
        recorder.As<SampleCapability>();

    ASSERT_NE(
        cap,
        nullptr);

    EXPECT_EQ(
        cap->value,
        7);
}

TEST(
    CallbackSubscriptionRegistryTest,
    OnlyMatchingCapabilityIsNotified)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder a;
    rim::CapabilityRecorder b;

    registry.Subscribe(
        kSlotA,
        rim::CapabilityRecorder::Callback,
        &a);

    registry.Subscribe(
        kSlotB,
        rim::CapabilityRecorder::Callback,
        &b);

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_EQ(a.calls.load(), 1);
    EXPECT_EQ(b.calls.load(), 0);
}

TEST(
    CallbackSubscriptionRegistryTest,
    MultipleSubscribersOnSameCapability)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    for (int i = 0; i < 3; ++i)
    {
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &recorder);
    }

    EXPECT_EQ(
        registry.Notify(
            kSlotA,
            Make(1)),
        3U);

    EXPECT_EQ(
        recorder.calls.load(),
        3);
}

TEST(
    CallbackSubscriptionRegistryTest,
    Unsubscribe)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    const auto id =
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &recorder);

    EXPECT_TRUE(
        registry.Unsubscribe(
            id));

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_FALSE(
        recorder.Called());
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

    rim::CapabilityRecorder removed;
    rim::CapabilityRecorder remaining;

    const auto first =
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &removed);

    registry.Subscribe(
        kSlotA,
        rim::CapabilityRecorder::Callback,
        &remaining);

    ASSERT_TRUE(
        registry.Unsubscribe(
            first));

    registry.Notify(
        kSlotA,
        Make(1));

    EXPECT_EQ(
        remaining.calls.load(),
        1);

    EXPECT_FALSE(
        removed.Called());

    EXPECT_EQ(
        registry.Count(),
        1U);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsNullCallback)
{
    rim::CallbackSubscriptionRegistry registry;

    EXPECT_EQ(
        registry.Subscribe(
            kSlotA,
            nullptr,
            nullptr),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsOutOfRangeCapabilityId)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    const rim::CapabilityId outOfRange =
        static_cast<rim::CapabilityId>(
            rim::kCapabilityMaxCount);

    EXPECT_EQ(
        registry.Subscribe(
            outOfRange,
            rim::CapabilityRecorder::Callback,
            &recorder),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    RejectsSubscriptionWhenFull)
{
    // 固定容量なので、満杯になったら採番失敗を返す(黙って伸びない)。
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    for (std::size_t i = 0;
         i < rim::kCapabilitySubscriptionMaxCount;
         ++i)
    {
        ASSERT_NE(
            registry.Subscribe(
                kSlotA,
                rim::CapabilityRecorder::Callback,
                &recorder),
            rim::kInvalidSubscriptionId);
    }

    EXPECT_EQ(
        registry.Subscribe(
            kSlotA,
            rim::CapabilityRecorder::Callback,
            &recorder),
        rim::kInvalidSubscriptionId);
}

TEST(
    CallbackSubscriptionRegistryTest,
    CountsSubscribersPerCapability)
{
    rim::CallbackSubscriptionRegistry registry;

    rim::CapabilityRecorder recorder;

    registry.Subscribe(
        kSlotA,
        rim::CapabilityRecorder::Callback,
        &recorder);

    registry.Subscribe(
        kSlotB,
        rim::CapabilityRecorder::Callback,
        &recorder);

    registry.Subscribe(
        kSlotB,
        rim::CapabilityRecorder::Callback,
        &recorder);

    EXPECT_EQ(
        registry.CountFor(kSlotA),
        1U);

    EXPECT_EQ(
        registry.CountFor(kSlotB),
        2U);
}
