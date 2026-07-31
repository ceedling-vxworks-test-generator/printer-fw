#include <gtest/gtest.h>

#include <cstdint>

#include "FixedQueue.hpp"

//
// 固定容量キュー。要点は2つ。
//   - 動的確保しない(std::queue はノードを確保する)
//   - 満杯時は **最も古いものを捨てて最新を残す**。捨てたことは観測できる。
//

TEST(
    FixedQueueTest,
    PushAndTryPop)
{
    rim::FixedQueue<std::int32_t, 4> q;

    q.Push(10);
    q.Push(20);

    std::int32_t out = 0;

    ASSERT_TRUE(q.TryPop(out));
    EXPECT_EQ(out, 10);

    ASSERT_TRUE(q.TryPop(out));
    EXPECT_EQ(out, 20);

    EXPECT_FALSE(q.TryPop(out));
}

TEST(
    FixedQueueTest,
    TryPopOnEmptyReturnsFalse)
{
    rim::FixedQueue<std::int32_t, 4> q;

    std::int32_t out = 0;

    EXPECT_FALSE(q.TryPop(out));
    EXPECT_TRUE(q.Empty());
}

TEST(
    FixedQueueTest,
    OverflowDropsOldestAndKeepsNewest)
{
    rim::FixedQueue<std::int32_t, 3> q;

    // 容量 +2 件積む
    for (std::int32_t i = 1; i <= 5; ++i)
    {
        q.Push(i);
    }

    EXPECT_EQ(q.Size(), 3U);

    // 捨てたことは観測できる
    EXPECT_EQ(q.DroppedCount(), 2U);

    // 残っているのは新しい方(3,4,5)
    std::int32_t out = 0;

    ASSERT_TRUE(q.TryPop(out));
    EXPECT_EQ(out, 3);

    ASSERT_TRUE(q.TryPop(out));
    EXPECT_EQ(out, 4);

    ASSERT_TRUE(q.TryPop(out));
    EXPECT_EQ(out, 5);
}

TEST(
    FixedQueueTest,
    WrapAroundKeepsOrder)
{
    // リングを1周させても順序が壊れないこと
    rim::FixedQueue<std::int32_t, 3> q;

    std::int32_t out = 0;

    for (std::int32_t i = 1; i <= 10; ++i)
    {
        q.Push(i);

        ASSERT_TRUE(q.TryPop(out));
        EXPECT_EQ(out, i);
    }

    EXPECT_TRUE(q.Empty());
    EXPECT_EQ(q.DroppedCount(), 0U);
}

TEST(
    FixedQueueTest,
    WaitAndPopReturnsQueuedItem)
{
    rim::FixedQueue<std::int32_t, 4> q;

    q.Push(42);

    std::int32_t out = 0;

    ASSERT_TRUE(q.WaitAndPop(out));
    EXPECT_EQ(out, 42);
}

TEST(
    FixedQueueTest,
    ShutdownReleasesWaiter)
{
    rim::FixedQueue<std::int32_t, 4> q;

    q.Shutdown();

    std::int32_t out = 0;

    // 空 + Shutdown 済みなら false(ワーカの終了条件)
    EXPECT_FALSE(q.WaitAndPop(out));
}

TEST(
    FixedQueueTest,
    ShutdownStillDrainsRemainingItems)
{
    // Shutdown してもキューに残っている分は取り出せること
    // (取りこぼしたまま終わらない)
    rim::FixedQueue<std::int32_t, 4> q;

    q.Push(1);
    q.Shutdown();

    std::int32_t out = 0;

    ASSERT_TRUE(q.WaitAndPop(out));
    EXPECT_EQ(out, 1);

    EXPECT_FALSE(q.WaitAndPop(out));
}
