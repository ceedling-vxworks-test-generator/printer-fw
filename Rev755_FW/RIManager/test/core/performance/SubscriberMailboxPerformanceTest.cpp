#include <gtest/gtest.h>

#include <chrono>

#include "rim_api.h"

#include "SubscriberMailbox.hpp"

#include "NotificationTrigger.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    SubscriberMailboxPerformanceTest,
    PushScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        100000,
        1000000
    };

    for (auto count : counts)
    {
        rim::SubscriberMailbox
            mailbox;

        rim::NotificationMessage message =
                test::PeriodicMessage(RI_CAPABILITY_ENVIRONMENT);

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            mailbox.Push(
                message);
        }

        auto end =
            std::chrono::steady_clock::now();

        std::cout
            << "[MailboxPush]"
            << " count=" << count
            << " elapsed="
            << std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                   .count()
            << " us"
            << std::endl;
    }
}

TEST(
    SubscriberMailboxPerformanceTest,
    PopScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        100000,
        1000000
    };

    for (auto count : counts)
    {
        rim::SubscriberMailbox
            mailbox;

        rim::NotificationMessage message =
                test::PeriodicMessage(RI_CAPABILITY_ENVIRONMENT);

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            mailbox.Push(message);
        }

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            mailbox.Pop(message);
        }

        auto end =
            std::chrono::steady_clock::now();

        std::cout
            << "[MailboxPop]"
            << " count=" << count
            << " elapsed="
            << std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                   .count()
            << " us"
            << std::endl;
    }
}