#include <gtest/gtest.h>

#include "StoreInputQueue.hpp"

TEST(
    StoreInputQueueTest,
    PushPop)
{
    rim::StoreInputQueue queue;

    rim::RIMDataItem input{};

    queue.Push(
        input);

    rim::RIMDataItem output{};

    EXPECT_TRUE(
        queue.TryPop(
            output));
}

TEST(
    StoreInputQueueTest,
    WaitAndPop)
{
    rim::StoreInputQueue queue;

    rim::RIMDataItem input{};

    queue.Push(
        input);

    rim::RIMDataItem output{};

    queue.WaitAndPop(
        output);
}

TEST(
    StoreInputQueueTest,
    Shutdown)
{
    rim::StoreInputQueue queue;

    queue.Shutdown();

    rim::RIMDataItem item{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            item));
}