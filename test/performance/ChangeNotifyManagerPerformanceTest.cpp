#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

#include "printer_a.h"

#include "ChangeNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

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
        rim::CallbackQueue callbackQueue;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

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

        rim::CallbackQueue callbackQueue;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

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

        rim::CallbackQueue callbackQueue;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

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

TEST(
    ChangeNotifyManagerPerformanceTest,
    MailboxSubscriberScale)
{
    const std::size_t counts[] =
    {
        10,
        100,
        1000,
        10000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore
            store;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::CallbackQueue callbackQueue;

        rim::ChangeNotifyManager
            manager(
                store,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

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

        manager.Notify(
        {
            rim::NotificationTargetType::
                Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::NotificationTrigger::
            Periodic);

        auto end =
            std::chrono::steady_clock::now();

        const auto elapsedUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                    .count();

        const double notifyPerSec =
            elapsedUs > 0
            ? (1000000.0 / elapsedUs)
            : 0.0;

        const double deliveryPerSec =
            elapsedUs > 0
            ? ((static_cast<double>(
                    count) * 1000000.0)
                / elapsedUs)
            : 0.0;

        std::cout
            << std::endl
            << "=== ChangeNotify Mailbox Scale ==="
            << std::endl
            << "Subscribers     : "
            << count
            << std::endl
            << "Notify/sec      : "
            << static_cast<
                std::uint64_t>(
                    notifyPerSec)
            << std::endl
            << "Delivery/sec    : "
            << static_cast<
                std::uint64_t>(
                    deliveryPerSec)
            << std::endl
            << "Elapsed(us)     : "
            << elapsedUs
            << std::endl;
    }
}

TEST(
    ChangeNotifyManagerPerformanceTest,
    NotifyBreakdownScaling)
{
    const std::size_t counts[] =
    {
        100,
        1000,
        5000,
        10000
    };

    for (auto count : counts)
    {
        rim::SubscriptionStore
            store;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

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

        std::vector<
            rim::SubscriptionInfo>
                subscriptions;

        uint64_t lookupUs = 0;
        uint64_t deliveryUs = 0;

        {
            auto start =
                std::chrono::steady_clock::now();

            store.GetSubscriptions(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_ENVIRONMENT
            },
            rim::NotificationTrigger::
                Periodic,
            subscriptions);

            auto end =
                std::chrono::steady_clock::now();

            lookupUs =
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        end - start)
                        .count();
        }

        {
            auto start =
                std::chrono::steady_clock::now();

            for (const auto& info
                : subscriptions)
            {
                mailboxManager.
                    GetMailbox(
                        info.id)
                    .Push(
                    {
                        {
                            rim::NotificationTargetType::
                                Capability,
                            RI_CAPABILITY_ENVIRONMENT
                        },
                        rim::NotificationTrigger::
                            Periodic
                    });
            }

            auto end =
                std::chrono::steady_clock::now();

            deliveryUs =
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        end - start)
                        .count();
        }

        std::cout
            << "[NotifyBreakdown]"
            << " subscribers="
            << count
            << " lookup="
            << lookupUs
            << " us"
            << " delivery="
            << deliveryUs
            << " us"
            << std::endl;
    }
}