#include <gtest/gtest.h>

#include "container/RingBuffer.hpp"
#include "container/FixedMap.hpp"
#include "container/FixedVector.hpp"

using namespace rim;

TEST(RingBufferTest, FifoOrderAndOverflow)
{
    RingBuffer<int, 3> rb;
    EXPECT_TRUE(rb.Push(1));
    EXPECT_TRUE(rb.Push(2));
    EXPECT_TRUE(rb.Push(3));
    EXPECT_FALSE(rb.Push(4));   // 満杯
    EXPECT_TRUE(rb.Full());

    int v = 0;
    EXPECT_TRUE(rb.Pop(v)); EXPECT_EQ(v, 1);
    EXPECT_TRUE(rb.Pop(v)); EXPECT_EQ(v, 2);
    EXPECT_TRUE(rb.Push(5));    // 空きができたので投入可
    EXPECT_TRUE(rb.Pop(v)); EXPECT_EQ(v, 3);
    EXPECT_TRUE(rb.Pop(v)); EXPECT_EQ(v, 5);
    EXPECT_FALSE(rb.Pop(v));    // 空
}

TEST(FixedMapTest, AddUpdateRemoveUniqueKey)
{
    FixedMap<int, int, 4> m;
    EXPECT_TRUE(m.Add(10, 100));
    EXPECT_FALSE(m.Add(10, 999));   // 重複キー
    EXPECT_EQ(*m.Find(10), 100);

    EXPECT_TRUE(m.Update(10, 200));
    EXPECT_EQ(*m.Find(10), 200);
    EXPECT_FALSE(m.Update(99, 1));  // 不在キー

    EXPECT_TRUE(m.Remove(10));
    EXPECT_FALSE(m.Contains(10));
    EXPECT_FALSE(m.Remove(10));     // 不在キー
}

TEST(FixedMapTest, CapacityLimit)
{
    FixedMap<int, int, 2> m;
    EXPECT_TRUE(m.Add(1, 1));
    EXPECT_TRUE(m.Add(2, 2));
    EXPECT_FALSE(m.Add(3, 3));      // 満杯
    EXPECT_EQ(m.Size(), 2u);
}

TEST(FixedVectorTest, PushAndCapacity)
{
    FixedVector<int, 2> v;
    EXPECT_TRUE(v.PushBack(7));
    EXPECT_TRUE(v.PushBack(8));
    EXPECT_FALSE(v.PushBack(9));    // 満杯
    EXPECT_EQ(v.Size(), 2u);
    EXPECT_EQ(v.At(0), 7);
    EXPECT_EQ(v.At(1), 8);
}
