#include <gtest/gtest.h>

#include "PublisherInputQueue.hpp"

TEST(
    PublisherInputQueueTest,
    PushPop)
{
    rim::PublisherInputQueue queue;

    rim::PublisherInput input{};

    queue.Push(
        input);

    rim::PublisherInput output{};

    EXPECT_TRUE(
        queue.TryPop(
            output));
}

TEST(
    PublisherInputQueueTest,
    WaitAndPop)
{
    rim::PublisherInputQueue queue;

    rim::PublisherInput input{};

    queue.Push(
        input);

    rim::PublisherInput output{};

    queue.WaitAndPop(
        output);
}

TEST(
    PublisherInputQueueTest,
    Shutdown)
{
    rim::PublisherInputQueue queue;

    queue.Shutdown();

    rim::PublisherInput input{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            input));
}

TEST(
    PublisherInputQueueTest,
    WaitAndPopForTimeout)
{
    rim::PublisherInputQueue queue;

    rim::PublisherInput input{};

    EXPECT_FALSE(
        queue.WaitAndPopFor(
            input,
            std::chrono::milliseconds(
                10)));
}

TEST(
    PublisherInputQueueTest,
    WaitAndPopFor)
{
    rim::PublisherInputQueue queue;

    rim::PublisherInput input{};

    queue.Push(
        rim::PublisherInput{});

    EXPECT_TRUE(
        queue.WaitAndPopFor(
            input,
            std::chrono::milliseconds(
                10)));
}