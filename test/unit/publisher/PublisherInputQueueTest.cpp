#include <gtest/gtest.h>

#include "PublisherInputQueue.hpp"

//
// 旧試験は CapabilityChangeSet(std::vector<std::string>)に "Environment" などの
// 文字列を積んでいた。現在は CapabilityChangeList(CapabilityId の固定長配列)。
//

namespace
{

constexpr rim::CapabilityId kSlotA = 0;
constexpr rim::CapabilityId kSlotB = 1;

}

TEST(
    PublisherInputQueueTest,
    PushPop)
{
    rim::PublisherInputQueue queue;

    rim::CapabilityChangeList input{};

    input.Add(
        kSlotA);

    queue.Push(
        input);

    rim::CapabilityChangeList output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    ASSERT_EQ(
        output.Size(),
        1U);

    EXPECT_TRUE(
        output.Contains(
            kSlotA));
}

TEST(
    PublisherInputQueueTest,
    WaitAndPop)
{
    rim::PublisherInputQueue queue;

    rim::CapabilityChangeList input{};

    input.Add(
        kSlotB);

    queue.Push(
        input);

    rim::CapabilityChangeList output{};

    ASSERT_TRUE(
        queue.WaitAndPop(
            output));

    ASSERT_EQ(
        output.Size(),
        1U);

    EXPECT_TRUE(
        output.Contains(
            kSlotB));
}

TEST(
    PublisherInputQueueTest,
    TryPopOnEmptyReturnsFalse)
{
    rim::PublisherInputQueue queue;

    rim::CapabilityChangeList output{};

    EXPECT_FALSE(
        queue.TryPop(
            output));
}

TEST(
    PublisherInputQueueTest,
    Shutdown)
{
    rim::PublisherInputQueue queue;

    queue.Shutdown();

    rim::CapabilityChangeList input{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            input));
}
