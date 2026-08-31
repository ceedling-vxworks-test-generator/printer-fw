#include <gtest/gtest.h>

#include "PublisherInputQueue.hpp"
#include "rim_api.h"
#include "test/core/support/NotificationTestHelper.hpp"

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

TEST(
    PublisherInputQueueTest,
    KeepLatestCompresses)
{
    rim::PublisherInputQueue queue;

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_ENVIRONMENT),
        rim::EventPriority::Low,
        rim::CompressionPolicy::KeepLatest
    });

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_ENVIRONMENT),
        rim::EventPriority::High,
        rim::CompressionPolicy::KeepLatest
    });

    size_t count = 0;

    rim::PublisherInput item{};

    while (queue.TryPop(item))
    {
        ++count;
    }

    EXPECT_EQ(1u, count);
}

TEST(
    PublisherInputQueueTest,
    KeepOldestCompresses)
{
    rim::PublisherInputQueue queue;

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_PRINT_READY),
        rim::EventPriority::Low,
        rim::CompressionPolicy::KeepOldest
    });

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_PRINT_READY),
        rim::EventPriority::High,
        rim::CompressionPolicy::KeepOldest
    });

    rim::PublisherInput item{};

    ASSERT_TRUE(
        queue.TryPop(item));

    EXPECT_EQ(
        rim::EventPriority::Low,
        item.priority);

    EXPECT_FALSE(
        queue.TryPop(item));
}

TEST(
    PublisherInputQueueTest,
    NonePolicyKeepsBothEvents)
{
    rim::PublisherInputQueue queue;

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_JOB),
        rim::EventPriority::Low,
        rim::CompressionPolicy::None
    });

    queue.Push({
        test::CapabilityTarget(
            RI_CAPABILITY_JOB),
        rim::EventPriority::High,
        rim::CompressionPolicy::None
    });

    std::size_t count = 0;

    rim::PublisherInput item{};

    while (queue.TryPop(item))
    {
        ++count;
    }

    EXPECT_EQ(
        2u,
        count);
}