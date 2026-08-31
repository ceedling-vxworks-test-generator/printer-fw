#include <gtest/gtest.h>

#include <chrono>

#include "rim_api.h"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "Product.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

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

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

        rim::PeriodicNotifyManager
            periodicNotifyManager;

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };

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
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
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

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

        rim::PeriodicNotifyManager
            periodicNotifyManager;

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };

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
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
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

TEST(
    PublishManagerPerformanceTest,
    DueScaling)
{
    const std::size_t totalSubscriptions =
    {
        5000
    };

    const std::size_t dueCounts[] =
    {
        1,
        10,
        100,
        1000,
        5000
    };

    for (auto dueCount : dueCounts)
    {
        rim::SubscriptionStore
            subscriptionStore;

        rim::SubscriberMailboxManager
            mailboxManager;

        rim::CallbackSubscriptionRegistry
            callbackRegistry;

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

        rim::PeriodicNotifyManager
            periodicNotifyManager;

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };

        const auto now =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < totalSubscriptions;
             ++i)
        {
            const auto id =
                static_cast<rim::SubscriptionId>(
                    i + 1);

            subscriptionStore.Register(
            {
                id,
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::Periodic
            });

            rim::PeriodicCondition
                condition{};

            condition.subscriptionId =
                id;

            condition.interval =
                std::chrono::seconds(10);

            if (i < dueCount)
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
            << "[DueScaling]"
            << " subscriptions="
            << totalSubscriptions
            << " due="
            << dueCount
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    PublishManagerPerformanceTest,
    MixedTargetScaling)
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

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

        rim::PeriodicNotifyManager
            periodicNotifyManager;
    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };

        const auto now =
            std::chrono::steady_clock::now();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            rim::RIMId
                target;

            switch (i % 4)
            {
            case 0:
                target =
                    test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT);
                break;

            case 1:
                target =
                    test::CapabilityTarget(RI_CAPABILITY_JOB);
                break;

            case 2:
                target =
                    test::CapabilityTarget(RI_CAPABILITY_PRINT_READY);
                break;

            default:
                target =
                    test::CapabilityTarget(RI_CAPABILITY_CONSUMABLE);
                break;
            }

            const auto id =
                static_cast<
                    rim::SubscriptionId>(
                        i + 1);

            subscriptionStore.Register(
            {
                id,
                target,
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
            << "[MixedTarget]"
            << " count="
            << count
            << " elapsed="
            << elapsedUs.count()
            << " us"
            << std::endl;
    }
}

TEST(
    PublishManagerPerformanceTest,
    BreakdownScaling)
{
    const std::size_t count =
        5000;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

        rim::CallbackQueue callbackQueue; 

        rim::ChangeNotifyManager
            notifyManager(
                subscriptionStore,
                mailboxManager,
                callbackRegistry,
                callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    const auto now =
        std::chrono::steady_clock::now();

    for (std::size_t i = 0;
         i < count;
         ++i)
    {
        const auto id =
            static_cast<
                rim::SubscriptionId>(
                    i + 1);

        subscriptionStore.Register(
        {
            id,
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
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

    uint64_t dueUs = 0;
    uint64_t findUs = 0;
    uint64_t notifyUs = 0;
    uint64_t updateUs = 0;

    std::vector<
        rim::PeriodicCondition>
            dueConditions;

    {
        auto start =
            std::chrono::steady_clock::now();

        periodicNotifyManager.
            GetDueConditions(
                dueConditions);

        auto end =
            std::chrono::steady_clock::now();

        dueUs =
            std::chrono::duration_cast<
                std::chrono::microseconds>(
                    end - start)
                    .count();
    }

    for (const auto& due
        : dueConditions)
    {
        rim::SubscriptionInfo
            info{};

        {
            auto start =
                std::chrono::steady_clock::now();

            subscriptionStore.Find(
                due.subscriptionId,
                info);

            auto end =
                std::chrono::steady_clock::now();

            findUs +=
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        end - start)
                        .count();
        }

        {
            auto start =
                std::chrono::steady_clock::now();

            notifyManager.Notify(
                info.target,
                rim::NotificationTrigger::
                    Periodic);

            auto end =
                std::chrono::steady_clock::now();

            notifyUs +=
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        end - start)
                        .count();
        }

        {
            auto start =
                std::chrono::steady_clock::now();

            periodicNotifyManager.
                UpdateNextTime(
                    due.subscriptionId);

            auto end =
                std::chrono::steady_clock::now();

            updateUs +=
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        end - start)
                        .count();
        }
    }

    std::cout
        << "[PublishBreakdown]"
        << " due="
        << dueConditions.size()
        << " getDue="
        << dueUs
        << " us"
        << " find="
        << findUs
        << " us"
        << " notify="
        << notifyUs
        << " us"
        << " update="
        << updateUs
        << " us"
        << std::endl;
}