#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "printer_a.h"

#include "SubscriptionStore.hpp"

#include "NotificationTargetType.hpp"
#include "DeliveryMethod.hpp"
#include "NotificationTrigger.hpp"

TEST(
    SubscriptionStorePerformanceTest,
    FindScaling)
{
    const std::size_t counts[] =
    {
        10,
        100,
        1000,
        5000,
        10000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore
            store;

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            store.Register(
            {
                static_cast<rim::SubscriptionId>(
                    i + 1),
                {
                    rim::NotificationTargetType::Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });
        }

        rim::SubscriptionInfo
            info{};

        auto start =
            std::chrono::steady_clock::now();

        store.Find(
            static_cast<rim::SubscriptionId>(
                count),
            info);

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[Find]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}
TEST(
    SubscriptionStorePerformanceTest,
    GetSubscriptionsScaling)
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
        rim::SubscriptionStore store;

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            store.Register(
            {
                static_cast<rim::SubscriptionId>(
                    i + 1),
                {
                    rim::NotificationTargetType::Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });
        }

        std::vector<rim::SubscriptionInfo>
            subscriptions;

        auto start =
            std::chrono::steady_clock::now();

        store.GetSubscriptions(
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::NotificationTrigger::Periodic,
        subscriptions);

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[GetSubscriptions]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    SubscriptionStorePerformanceTest,
    RemoveScaling)
{
    const std::size_t counts[] =
    {
        10,
        100,
        1000,
        5000,
        10000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore store;

    for (std::size_t i = 0;
        i < count;
        ++i)
    {
        store.Register(
        {
            static_cast<rim::SubscriptionId>(
                i + 1),
            {
                rim::NotificationTargetType::Capability,
                RI_CAPABILITY_ENVIRONMENT
            },
            rim::DeliveryMethod::Mailbox,
            rim::NotificationTrigger::Periodic
        });
    }

        auto start =
            std::chrono::steady_clock::now();

        store.Remove(
            static_cast<
                rim::SubscriptionId>(
                    count));

        auto end =
            std::chrono::steady_clock::now();

        std::cout
            << "[Remove]"
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
    SubscriptionStorePerformanceTest,
    RegisterScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        10000,
        50000,
        100000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore
            store;

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            store.Register(
            {
                static_cast<rim::SubscriptionId>(
                    i + 1),
                {
                    rim::NotificationTargetType::
                        Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::DeliveryMethod::
                    Mailbox,
                rim::NotificationTrigger::
                    Periodic
            });
        }

        auto end =
            std::chrono::steady_clock::now();

        std::cout
            << "[Register]"
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