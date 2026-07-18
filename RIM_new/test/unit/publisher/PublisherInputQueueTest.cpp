#include <gtest/gtest.h>

#include "publisher/PublisherInputQueue.hpp"

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