#include <gtest/gtest.h>

#include <cstdint>

#include "CapabilityStore.hpp"

//
// CapabilityStore は中身を解釈しない。
// 「何番のスロットに、どんなバイト列が入ったか」だけを試験する。
// (旧 MachineCapabilityStore は EnvironmentCapability 等を直接持っていたため、
//  Core の試験なのに製品の型を知っている必要があった)
//

namespace
{

// 試験用のダミー Capability。Core から見れば単なる trivially copyable な型。
struct SampleCapability
{
    double       value{};
    std::int32_t count{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

}

TEST(
    CapabilityStoreTest,
    StoreAndGet)
{
    rim::CapabilityStore store;

    SampleCapability input{};

    input.value = 300.15;
    input.count = 7;

    ASSERT_TRUE(
        store.Store(
            kSlotA,
            rim::CapabilityPayload::From(
                input)));

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        store.TryGet(
            kSlotA,
            payload));

    const auto* output =
        payload.As<SampleCapability>();

    ASSERT_NE(
        output,
        nullptr);

    EXPECT_DOUBLE_EQ(
        output->value,
        300.15);

    EXPECT_EQ(
        output->count,
        7);
}

TEST(
    CapabilityStoreTest,
    UnstoredSlotIsNotPresent)
{
    rim::CapabilityStore store;

    rim::CapabilityPayload payload;

    EXPECT_FALSE(
        store.Has(
            kSlotA));

    EXPECT_FALSE(
        store.TryGet(
            kSlotA,
            payload));
}

TEST(
    CapabilityStoreTest,
    SlotsAreIndependent)
{
    rim::CapabilityStore store;

    SampleCapability a{};
    a.count = 1;

    SampleCapability b{};
    b.count = 2;

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(a));

    store.Store(
        kSlotB,
        rim::CapabilityPayload::From(b));

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        store.TryGet(
            kSlotA,
            payload));

    EXPECT_EQ(
        payload.As<SampleCapability>()->count,
        1);

    ASSERT_TRUE(
        store.TryGet(
            kSlotB,
            payload));

    EXPECT_EQ(
        payload.As<SampleCapability>()->count,
        2);
}

TEST(
    CapabilityStoreTest,
    StoreOverwrites)
{
    rim::CapabilityStore store;

    SampleCapability first{};
    first.count = 1;

    SampleCapability second{};
    second.count = 99;

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(first));

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(second));

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        store.TryGet(
            kSlotA,
            payload));

    EXPECT_EQ(
        payload.As<SampleCapability>()->count,
        99);
}

TEST(
    CapabilityStoreTest,
    OutOfRangeIdIsRejected)
{
    rim::CapabilityStore store;

    SampleCapability input{};

    const rim::CapabilityId outOfRange =
        static_cast<rim::CapabilityId>(
            rim::kCapabilityMaxCount);

    EXPECT_FALSE(
        store.Store(
            outOfRange,
            rim::CapabilityPayload::From(
                input)));

    rim::CapabilityPayload payload;

    EXPECT_FALSE(
        store.TryGet(
            outOfRange,
            payload));
}

TEST(
    CapabilityStoreTest,
    ClearRemovesEverything)
{
    rim::CapabilityStore store;

    SampleCapability input{};

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    store.Clear();

    EXPECT_FALSE(
        store.Has(
            kSlotA));
}
