#include <gtest/gtest.h>

#include "PublisherInputQueue.hpp"

TEST(
    PublisherInputQueueTest,
    PushPop)
{
    rim::PublisherInputQueue queue;

    rim::CapabilityChangeSet input{};

    input.changedCapabilities.push_back(
        "Environment");

    queue.Push(
        input);

    rim::CapabilityChangeSet output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    ASSERT_EQ(
        output.changedCapabilities.size(),
        1U);

    EXPECT_EQ(
        output.changedCapabilities.front(),
        "Environment");
}

TEST(
    PublisherInputQueueTest,
    WaitAndPop)
{
    rim::PublisherInputQueue queue;

    rim::CapabilityChangeSet input{};

    input.changedCapabilities.push_back(
        "Error");

    queue.Push(
        input);

    rim::CapabilityChangeSet output{};

    ASSERT_TRUE(
        queue.WaitAndPop(
            output));

    ASSERT_EQ(
        output.changedCapabilities.size(),
        1U);

    EXPECT_EQ(
        output.changedCapabilities.front(),
        "Error");
}

TEST(
    PublisherInputQueueTest,
    Shutdown)
{
    rim::PublisherInputQueue queue;

    queue.Shutdown();

    rim::CapabilityChangeSet input{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            input));
}