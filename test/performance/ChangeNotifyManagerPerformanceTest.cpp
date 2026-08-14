#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "ChangeNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "rim_capability_id.h"

TEST(
    ChangeNotifyManagerPerformanceTest,
    NotifyScaling)
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
            store;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry);

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

        manager.Notify(
        {
            rim::NotificationTargetType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        },
        rim::NotificationTrigger::Periodic);

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[Notify]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    ChangeNotifyManagerPerformanceTest,
    NotifyRepeatedScaling)
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
            store;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry);

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            store.Register(
            {
                static_cast<
                    rim::SubscriptionId>(
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

        auto start =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            manager.Notify(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_ENVIRONMENT
            },
            rim::NotificationTrigger::
                Periodic);
        }

        auto end =
            std::chrono::steady_clock::now();

        std::cout
            << "[NotifyRepeated]"
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
    ChangeNotifyManagerPerformanceTest,
    NotifySingleSubscriber)
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
            store;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry);

        //
        // Environment購読
        // 1件だけ
        //
        store.Register(
        {
            1,
            {
                rim::NotificationTargetType::Capability,
                RI_CAPABILITY_ENVIRONMENT
            },
            rim::DeliveryMethod::Mailbox,
            rim::NotificationTrigger::Periodic
        });

        //
        // 残りは別Target
        //
        for (std::size_t i = 1;
             i < count;
             ++i)
        {
            store.Register(
            {
                static_cast<rim::SubscriptionId>(
                    i + 1),
                {
                    rim::NotificationTargetType::Capability,
                    RI_CAPABILITY_JOB
                },
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });
        }

        auto start =
            std::chrono::steady_clock::now();

        manager.Notify(
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::NotificationTrigger::Periodic);

        auto end =
            std::chrono::steady_clock::now();

        auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start);

        std::cout
            << "[NotifySingle]"
            << " count=" << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}