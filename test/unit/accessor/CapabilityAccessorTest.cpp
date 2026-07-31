#include <gtest/gtest.h>

#include <cstdint>

#include "CapabilityAccessor.hpp"
#include "CapabilityStore.hpp"

//
// 「現在値」の読み出し経路。
//
// 旧実装は GetEnvironment() / GetError() / GetJob() と Capability ごとに関数を
// 持っていた。現在は id を渡して、型消去のまま(TryGet)か、指定の型として
// (TryGetAs<T>)取り出す。
//

namespace
{

struct SampleCapability
{
    double       temperature{};
    std::int32_t level{};
};

// サイズの異なる別型(取り違え検知の確認用)
struct OtherCapability
{
    bool flag{};
};

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

}

TEST(
    CapabilityAccessorTest,
    TryGetAsReturnsStoredValue)
{
    rim::CapabilityStore store;

    SampleCapability input{};

    input.temperature = 35.0;
    input.level       = 80;

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::CapabilityAccessor accessor(
        store);

    SampleCapability output{};

    ASSERT_TRUE(
        accessor.TryGetAs(
            kSlotA,
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        35.0);

    EXPECT_EQ(
        output.level,
        80);
}

TEST(
    CapabilityAccessorTest,
    TryGetReturnsRawPayload)
{
    rim::CapabilityStore store;

    SampleCapability input{};

    input.level = 5;

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::CapabilityAccessor accessor(
        store);

    rim::CapabilityPayload payload;

    ASSERT_TRUE(
        accessor.TryGet(
            kSlotA,
            payload));

    EXPECT_EQ(
        payload.Size(),
        sizeof(SampleCapability));
}

TEST(
    CapabilityAccessorTest,
    UnstoredCapabilityReturnsFalse)
{
    rim::CapabilityStore store;

    rim::CapabilityAccessor accessor(
        store);

    SampleCapability output{};

    EXPECT_FALSE(
        accessor.Has(
            kSlotB));

    EXPECT_FALSE(
        accessor.TryGetAs(
            kSlotB,
            output));
}

TEST(
    CapabilityAccessorTest,
    WrongSizeTypeIsRejected)
{
    // 型消去なのでサイズ違いは検知できる(同サイズの別型は検知できない)。
    rim::CapabilityStore store;

    SampleCapability input{};

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(
            input));

    rim::CapabilityAccessor accessor(
        store);

    OtherCapability wrong{};

    EXPECT_FALSE(
        accessor.TryGetAs(
            kSlotA,
            wrong));
}

TEST(
    CapabilityAccessorTest,
    ReadsTheLatestValue)
{
    // 通知と違い、こちらは何度読んでも「今の値」が返る(消費しない)。
    rim::CapabilityStore store;

    SampleCapability first{};
    first.level = 1;

    SampleCapability second{};
    second.level = 2;

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(first));

    rim::CapabilityAccessor accessor(
        store);

    SampleCapability output{};

    ASSERT_TRUE(
        accessor.TryGetAs(kSlotA, output));

    EXPECT_EQ(output.level, 1);

    store.Store(
        kSlotA,
        rim::CapabilityPayload::From(second));

    ASSERT_TRUE(
        accessor.TryGetAs(kSlotA, output));

    EXPECT_EQ(output.level, 2);

    // 2回目も同じ値が読める(消費されない)
    ASSERT_TRUE(
        accessor.TryGetAs(kSlotA, output));

    EXPECT_EQ(output.level, 2);
}
