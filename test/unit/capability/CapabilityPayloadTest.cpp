#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "CapabilityPayload.hpp"

//
// 型消去バイト列の性質を確認する。
//
// 旧 CapabilityDiffChecker は Capability の型ごとに HasChanged を書いていたが、
// 差分判定は CapabilityPayload のバイト比較へ一本化した。バイト比較が信頼できる
// ことが前提なので、**パディングの影響を受けない**ことをここで担保する。
//

namespace
{

struct Plain
{
    double       value{};
    std::int32_t count{};
};

// パディングが入る型(bool の後ろに3バイトの穴)。
// memset でパディングを汚す試験をするため、既定初期化子は付けない
// (付けると非トリビアル型になり -Wclass-memaccess で警告になる)。
struct Padded
{
    bool         flag;
    std::int32_t count;
};

}

TEST(
    CapabilityPayloadTest,
    RoundTripsTheStoredValue)
{
    Plain input{};

    input.value = 300.15;
    input.count = 7;

    const auto payload =
        rim::CapabilityPayload::From(
            input);

    const auto* output =
        payload.As<Plain>();

    ASSERT_NE(
        output,
        nullptr);

    EXPECT_DOUBLE_EQ(
        output->value,
        300.15);

    EXPECT_EQ(
        output->count,
        7);

    EXPECT_EQ(
        payload.Size(),
        sizeof(Plain));
}

TEST(
    CapabilityPayloadTest,
    SameValueCompresEqual)
{
    Plain a{};
    a.value = 1.0;
    a.count = 2;

    Plain b{};
    b.value = 1.0;
    b.count = 2;

    EXPECT_TRUE(
        rim::CapabilityPayload::From(a)
            .Equals(
                rim::CapabilityPayload::From(b)));
}

TEST(
    CapabilityPayloadTest,
    DifferentValueComparesUnequal)
{
    Plain a{};
    a.count = 1;

    Plain b{};
    b.count = 2;

    EXPECT_FALSE(
        rim::CapabilityPayload::From(a)
            .Equals(
                rim::CapabilityPayload::From(b)));
}

TEST(
    CapabilityPayloadTest,
    PaddingDoesNotAffectComparison)
{
    // パディングにゴミが残っていると、論理的に同じ値でもバイト比較が
    // 食い違う。From() が 0 埋めしていることを、意図的にゴミを入れて確認する。
    Padded dirty;

    std::memset(
        &dirty,
        0xFF,
        sizeof dirty);

    dirty.flag  = true;
    dirty.count = 5;

    Padded clean{};

    clean.flag  = true;
    clean.count = 5;

    EXPECT_TRUE(
        rim::CapabilityPayload::From(dirty)
            .Equals(
                rim::CapabilityPayload::From(clean)));
}

TEST(
    CapabilityPayloadTest,
    WrongSizeTypeIsRejected)
{
    Plain input{};

    const auto payload =
        rim::CapabilityPayload::From(
            input);

    // サイズが違う型では取り出せない。
    // (同サイズの別型は検知できない。これは型消去の代償として承知の上)
    EXPECT_EQ(
        payload.As<Padded>(),
        nullptr);
}

TEST(
    CapabilityPayloadTest,
    DefaultConstructedIsEmpty)
{
    rim::CapabilityPayload payload;

    EXPECT_TRUE(
        payload.Empty());

    EXPECT_EQ(
        payload.Size(),
        0U);
}
