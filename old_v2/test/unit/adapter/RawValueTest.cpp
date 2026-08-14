#include <gtest/gtest.h>

#include <cstdint>

#include "RawValue.hpp"

//
// 受理点が「型自由」であることを支える生値の器。
// スカラだけでなく構造体・配列も **コピーせず参照で** 受けられる。
//

namespace
{

struct SampleStruct
{
    std::int32_t a;
    double       b;
};

}

TEST(
    RawValueTest,
    Scalar)
{
    const auto raw =
        rim::RawValue::Scalar(1.5);

    EXPECT_EQ(
        raw.kind,
        rim::RawValue::Kind::kScalar);

    EXPECT_DOUBLE_EQ(
        raw.scalar,
        1.5);
}

TEST(
    RawValueTest,
    StructIsReferencedNotCopied)
{
    SampleStruct s{7, 2.5};

    const auto raw =
        rim::RawValue::Struct(s);

    EXPECT_EQ(
        raw.kind,
        rim::RawValue::Kind::kBytes);

    EXPECT_EQ(
        raw.size,
        sizeof(SampleStruct));

    EXPECT_EQ(
        raw.count,
        1U);

    // 元の実体を指している(コピーしていない)
    EXPECT_EQ(
        raw.data,
        static_cast<const void*>(&s));

    const auto* view =
        raw.As<SampleStruct>();

    ASSERT_NE(
        view,
        nullptr);

    EXPECT_EQ(view->a, 7);
    EXPECT_DOUBLE_EQ(view->b, 2.5);
}

TEST(
    RawValueTest,
    Array)
{
    const std::int32_t values[3] = {1, 2, 3};

    const auto raw =
        rim::RawValue::Array(
            values,
            3);

    EXPECT_EQ(
        raw.kind,
        rim::RawValue::Kind::kBytes);

    EXPECT_EQ(
        raw.count,
        3U);

    EXPECT_EQ(
        raw.size,
        sizeof(std::int32_t) * 3);

    EXPECT_EQ(
        raw.ElementAt<std::int32_t>(0),
        1);

    EXPECT_EQ(
        raw.ElementAt<std::int32_t>(2),
        3);
}

TEST(
    RawValueTest,
    DefaultIsScalarZero)
{
    const rim::RawValue raw{};

    EXPECT_EQ(
        raw.kind,
        rim::RawValue::Kind::kScalar);

    EXPECT_DOUBLE_EQ(
        raw.scalar,
        0.0);
}
