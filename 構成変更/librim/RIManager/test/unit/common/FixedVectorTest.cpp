#include <gtest/gtest.h>

#include <cstdint>

#include "FixedVector.hpp"

//
// 固定容量配列。std::vector と違い **黙って伸びない** ことが要点。
//

TEST(
    FixedVectorTest,
    PushBackAndIndex)
{
    rim::FixedVector<std::int32_t, 4> v;

    ASSERT_TRUE(v.PushBack(10));
    ASSERT_TRUE(v.PushBack(20));

    EXPECT_EQ(v.Size(), 2U);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
}

TEST(
    FixedVectorTest,
    PushBackFailsWhenFull)
{
    rim::FixedVector<std::int32_t, 2> v;

    ASSERT_TRUE(v.PushBack(1));
    ASSERT_TRUE(v.PushBack(2));

    EXPECT_TRUE(v.Full());

    // 満杯なら失敗する(容量が勝手に増えない)
    EXPECT_FALSE(v.PushBack(3));

    EXPECT_EQ(v.Size(), 2U);
    EXPECT_EQ(v[1], 2);
}

TEST(
    FixedVectorTest,
    EraseKeepsOrder)
{
    rim::FixedVector<std::int32_t, 4> v;

    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);

    ASSERT_TRUE(v.Erase(1));

    ASSERT_EQ(v.Size(), 2U);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 3);
}

TEST(
    FixedVectorTest,
    EraseOutOfRangeReturnsFalse)
{
    rim::FixedVector<std::int32_t, 4> v;

    v.PushBack(1);

    EXPECT_FALSE(v.Erase(1));
    EXPECT_EQ(v.Size(), 1U);
}

TEST(
    FixedVectorTest,
    PopBack)
{
    rim::FixedVector<std::int32_t, 4> v;

    v.PushBack(1);
    v.PushBack(2);

    ASSERT_TRUE(v.PopBack());

    EXPECT_EQ(v.Size(), 1U);
    EXPECT_EQ(v[0], 1);

    ASSERT_TRUE(v.PopBack());

    EXPECT_TRUE(v.Empty());
    EXPECT_FALSE(v.PopBack());
}

TEST(
    FixedVectorTest,
    RangeForWorks)
{
    rim::FixedVector<std::int32_t, 4> v;

    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);

    std::int32_t sum = 0;

    for (const auto& value : v)
    {
        sum += value;
    }

    EXPECT_EQ(sum, 6);
}

TEST(
    FixedVectorTest,
    Clear)
{
    rim::FixedVector<std::int32_t, 4> v;

    v.PushBack(1);
    v.PushBack(2);

    v.Clear();

    EXPECT_TRUE(v.Empty());
    EXPECT_EQ(v.Size(), 0U);
}
