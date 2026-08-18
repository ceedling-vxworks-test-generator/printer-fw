#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "rim_capability_id.h"

TEST(
    PublishManagerPerformanceTest,
    ProcessPeriodicNotificationsScaling)
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
        rim::SubscriptionStore
            subscriptionStore;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry);

        rim::PeriodicNotifyManager
            periodicNotifyManager;

        rim::PublishManager
            publishManager(
                notifyManager,
                periodicNotifyManager,
                subscriptionStore);

        const auto now =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            const auto id =
                static_cast<rim::SubscriptionId>(
                    i + 1);

            subscriptionStore.Register(
            {
                id,
                {
                    rim::NotificationTargetType::Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });

            rim::PeriodicCondition
                condition{};

            condition.subscriptionId =
                id;

            condition.interval =
                std::chrono::seconds(10);

            condition.nextNotifyTime =
                now -
                std::chrono::seconds(1);

            periodicNotifyManager.Register(
                condition);
        }

        auto start =
            std::chrono::steady_clock::now();

        publishManager.
            ProcessPeriodicNotifications();

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[PublishManager]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    PublishManagerPerformanceTest,
    PartialDueScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        5000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore
            subscriptionStore;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry);

        rim::PeriodicNotifyManager
            periodicNotifyManager;

        rim::PublishManager
            publishManager(
                notifyManager,
                periodicNotifyManager,
                subscriptionStore);

        const auto now =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            const auto id =
                static_cast<rim::SubscriptionId>(
                    i + 1);

            subscriptionStore.Register(
            {
                id,
                {
                    rim::NotificationTargetType::Capability,
                    RI_CAPABILITY_ENVIRONMENT
                },
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });

            rim::PeriodicCondition
                condition{};

            condition.subscriptionId =
                id;

            condition.interval =
                std::chrono::seconds(10);

            if (i < 10)
            {
                condition.nextNotifyTime =
                    now -
                    std::chrono::seconds(1);
            }
            else
            {
                condition.nextNotifyTime =
                    now +
                    std::chrono::hours(1);
            }

            periodicNotifyManager.Register(
                condition);
        }

        auto start =
            std::chrono::steady_clock::now();

        publishManager.
            ProcessPeriodicNotifications();

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[PartialDue]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}