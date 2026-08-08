#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "PeriodicNotifyManager.hpp"

TEST(
    PeriodicNotifyManagerPerformanceTest,
    UpdateAllScaling)
{
    const std::size_t counts[] =
    {
        10,
        100,
        1000,
        5000
    };

    for (auto count : counts)
    {
        rim::PeriodicNotifyManager
            manager;

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            rim::PeriodicCondition
                condition{};

            condition.subscriptionId =
                static_cast<
                    rim::SubscriptionId>(
                        i + 1);

            condition.interval =
                std::chrono::seconds(1);

            condition.nextNotifyTime =
                std::chrono::steady_clock::now();

            manager.Register(
                condition);
        }

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            manager.UpdateNextTime(
                static_cast<
                    rim::SubscriptionId>(
                        i + 1));
        }

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[UpdateAll]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    PeriodicNotifyManagerPerformanceTest,
    UpdateLastElementScaling)
{
    const std::size_t counts[] =
    {
        10,
        100,
        1000,
        5000,
        10000,
        50000
    };

    for (auto count : counts)
    {
        rim::PeriodicNotifyManager
            manager;

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            rim::PeriodicCondition
                condition{};

            condition.subscriptionId =
                static_cast<
                    rim::SubscriptionId>(
                        i + 1);

            condition.interval =
                std::chrono::seconds(1);

            condition.nextNotifyTime =
                std::chrono::steady_clock::now();

            manager.Register(
                condition);
        }

        auto start =
            std::chrono::steady_clock::now();

        manager.UpdateNextTime(
            static_cast<
                rim::SubscriptionId>(
                    count));

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[UpdateLast]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}