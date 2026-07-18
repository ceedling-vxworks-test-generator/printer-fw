#include <gtest/gtest.h>

#include "capability/CapabilityInputQueue.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::CapabilityInputQueue queue;

    rim::RIMSnapshot input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    queue.Push(
        input);

    rim::RIMSnapshot output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output.humidity,
        60.0);
}

TEST(
    CapabilityInputQueueTest,
    WaitAndPop)
{
    rim::CapabilityInputQueue queue;

    rim::RIMSnapshot input{};

    queue.Push(
        input);

    rim::RIMSnapshot output{};

    queue.WaitAndPop(
        output);
}

TEST(
    CapabilityInputQueueTest,
    Shutdown)
{
    rim::CapabilityInputQueue queue;

    queue.Shutdown();

    rim::RIMSnapshot snapshot{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            snapshot));
}