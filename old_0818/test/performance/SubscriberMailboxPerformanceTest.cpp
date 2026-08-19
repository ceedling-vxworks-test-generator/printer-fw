#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "printer_a.h"

#include "SubscriberMailbox.hpp"

#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"

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

        rim::NotificationMessage
            message
            {
                {
                    rim::NotificationTargetType::
                        Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::NotificationTrigger::
                    Periodic
            };

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

        rim::NotificationMessage
            message
            {
                {
                    rim::NotificationTargetType::
                        Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::NotificationTrigger::
                    Periodic
            };

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            mailbox.Push(
                message);
        }

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            mailbox.Pop(
                message);
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