#include <gtest/gtest.h>

#include "RIManager.hpp"

//
// 利用者向けファサード。
// 旧実装の TryGetEnvironment などは TryGet / TryGetAs<T> に統合されている。
//

namespace
{

struct SampleCapability
{
    double temperature{};
    double humidity{};
};

struct OtherCapability
{
    bool flag{};
};

constexpr rim::CapabilityId kSlotA = 0;

}

TEST(
    RIManagerFacadeTest,
    TryGetAsReturnsPushedNotification)
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

    rim::RIManager facade(
        receiver);

    SampleCapability output{};

    ASSERT_TRUE(
        facade.TryGetAs(
            kSlotA,
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output.humidity,
        60.0);
}

TEST(
    RIManagerFacadeTest,
    NotificationIsConsumed)
{
    // 通知は1回取り出したら消える(現在値の読み出しとは役割が違う)。
    rim::SubscriberMailbox mailbox;

    SampleCapability input{};

    mailbox.Push(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::NotificationReceiver receiver(
        mailbox);

    rim::RIManager facade(
        receiver);

    SampleCapability output{};

    ASSERT_TRUE(
        facade.TryGetAs(
            kSlotA,
            output));

    EXPECT_FALSE(
        facade.TryGetAs(
            kSlotA,
            output));
}

TEST(
    RIManagerFacadeTest,
    WrongSizeTypeIsRejected)
{
    rim::SubscriberMailbox mailbox;

    SampleCapability input{};

    mailbox.Push(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::NotificationReceiver receiver(
        mailbox);

    rim::RIManager facade(
        receiver);

    OtherCapability wrong{};

    EXPECT_FALSE(
        facade.TryGetAs(
            kSlotA,
            wrong));
}

TEST(
    RIManagerFacadeTest,
    TryGetOnEmptyReturnsFalse)
{
    rim::SubscriberMailbox mailbox;

    rim::NotificationReceiver receiver(
        mailbox);

    rim::RIManager facade(
        receiver);

    rim::CapabilityPayload payload;

    EXPECT_FALSE(
        facade.TryGet(
            kSlotA,
            payload));
}
