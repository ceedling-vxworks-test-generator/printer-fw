#include <gtest/gtest.h>

#include <cstdint>

#include "SubscriberMailbox.hpp"

//
// 通知の一時保持。
//
// 旧実装は std::queue<EnvironmentCapability> 等を種別ごとに持っていた
// (動的確保あり・容量無制限)。現在は id 添字の固定長リングで、
// 溢れたら最も古い通知を捨てる。ここではその境界も含めて確認する。
//

namespace
{

struct SampleCapability
{
    std::int32_t seq{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

rim::CapabilityPayload Make(
    std::int32_t seq)
{
    SampleCapability cap{};

    cap.seq = seq;

    return rim::CapabilityPayload::From(
        cap);
}

std::int32_t SeqOf(
    const rim::CapabilityPayload& payload)
{
    const auto* cap =
        payload.As<SampleCapability>();

    return cap != nullptr
        ? cap->seq
        : -1;
}

}

TEST(
    SubscriberMailboxTest,
    PushAndPop)
{
    rim::SubscriberMailbox mailbox;

    ASSERT_TRUE(
        mailbox.Push(
            kSlotA,
            Make(1)));

    rim::CapabilityPayload output;

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotA,
            output));

    EXPECT_EQ(
        SeqOf(output),
        1);
}

TEST(
    SubscriberMailboxTest,
    PopOnEmptyReturnsFalse)
{
    rim::SubscriberMailbox mailbox;

    rim::CapabilityPayload output;

    EXPECT_FALSE(
        mailbox.Pop(
            kSlotA,
            output));
}

TEST(
    SubscriberMailboxTest,
    PreservesOrderWithinOneCapability)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(kSlotA, Make(1));
    mailbox.Push(kSlotA, Make(2));

    rim::CapabilityPayload output;

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotA,
            output));

    EXPECT_EQ(
        SeqOf(output),
        1);

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotA,
            output));

    EXPECT_EQ(
        SeqOf(output),
        2);
}

TEST(
    SubscriberMailboxTest,
    CapabilitiesDoNotInterfere)
{
    // 片方を読まなくても、もう片方が詰まらないこと。
    rim::SubscriberMailbox mailbox;

    mailbox.Push(kSlotA, Make(1));
    mailbox.Push(kSlotB, Make(2));

    rim::CapabilityPayload output;

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotB,
            output));

    EXPECT_EQ(
        SeqOf(output),
        2);

    // A は手つかずのまま残っている
    EXPECT_EQ(
        mailbox.Count(
            kSlotA),
        1U);
}

TEST(
    SubscriberMailboxTest,
    OverflowDropsOldestAndKeepsNewest)
{
    rim::SubscriberMailbox mailbox;

    // 容量 +2 件積む
    const std::int32_t total =
        static_cast<std::int32_t>(
            rim::kCapabilityMailboxDepth) + 2;

    for (std::int32_t i = 0; i < total; ++i)
    {
        mailbox.Push(
            kSlotA,
            Make(i));
    }

    // 容量以上には溜まらない
    EXPECT_EQ(
        mailbox.Count(
            kSlotA),
        rim::kCapabilityMailboxDepth);

    // 捨てられたのは古い方(先頭は 2 番目以降になっている)
    rim::CapabilityPayload output;

    ASSERT_TRUE(
        mailbox.Pop(
            kSlotA,
            output));

    EXPECT_EQ(
        SeqOf(output),
        2);

    // 捨てたことは観測できる
    EXPECT_EQ(
        mailbox.DroppedCount(
            kSlotA),
        2U);
}

TEST(
    SubscriberMailboxTest,
    OutOfRangeIdIsRejected)
{
    rim::SubscriberMailbox mailbox;

    const rim::CapabilityId outOfRange =
        static_cast<rim::CapabilityId>(
            rim::kCapabilityMaxCount);

    EXPECT_FALSE(
        mailbox.Push(
            outOfRange,
            Make(1)));

    rim::CapabilityPayload output;

    EXPECT_FALSE(
        mailbox.Pop(
            outOfRange,
            output));
}

TEST(
    SubscriberMailboxTest,
    ClearEmptiesEverything)
{
    rim::SubscriberMailbox mailbox;

    mailbox.Push(kSlotA, Make(1));
    mailbox.Push(kSlotB, Make(2));

    mailbox.Clear();

    EXPECT_TRUE(
        mailbox.Empty(
            kSlotA));

    EXPECT_TRUE(
        mailbox.Empty(
            kSlotB));
}
