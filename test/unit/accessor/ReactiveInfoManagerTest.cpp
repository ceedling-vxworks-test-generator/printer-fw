#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

//
// ポーリング購読の受け口。
// 旧実装の TryGetEnvironment / TryGetError / TryGetPrintReady は
// TryGet(id, payload) の1本に統合されている。
//

namespace
{

struct SampleCapability
{
    double temperature{};
    double humidity{};
};

constexpr rim::CapabilityId kSlotA = 0;

}

TEST(
    NotificationReceiverTest,
    TryGetReturnsPushedNotification)
{
    rim::SubscriberMailbox mailbox;

    SampleCapability input{};

    input.temperature = 300.15;
    input.humidity    = 60.0;

    mailbox.Push(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::NotificationReceiver receiver(
        mailbox);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        receiver.TryGet(
            kSlotA,
            payload));

    const auto* output =
        payload.As<SampleCapability>();

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
    NotificationReceiverTest,
    TryGetOnEmptyReturnsFalse)
{
    rim::SubscriberMailbox mailbox;

    rim::NotificationReceiver receiver(
        mailbox);

    rim::CapabilityPayload payload;

    EXPECT_FALSE(
        receiver.HasPending(
            kSlotA));

    EXPECT_FALSE(
        receiver.TryGet(
            kSlotA,
            payload));
}

TEST(
    NotificationReceiverTest,
    PendingCountReflectsMailbox)
{
    rim::SubscriberMailbox mailbox;

    SampleCapability input{};

    mailbox.Push(
        kSlotA,
        rim::CapabilityPayload::From(input));

    mailbox.Push(
        kSlotA,
        rim::CapabilityPayload::From(input));

    rim::NotificationReceiver receiver(
        mailbox);

    EXPECT_EQ(
        receiver.Pending(
            kSlotA),
        2U);

    rim::CapabilityPayload payload;

    receiver.TryGet(
        kSlotA,
        payload);

    EXPECT_EQ(
        receiver.Pending(
            kSlotA),
        1U);
}
