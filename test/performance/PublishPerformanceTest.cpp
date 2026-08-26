#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <atomic>
#include <thread>

#include "printer_a.h"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "SubscriberMailbox.hpp"
#include "PeriodicNotifyManager.hpp"
#include "NotificationTargetType.hpp"
#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"
#include "test/support/TestWaitHelper.hpp"
#include "PrinterAProductDefinition.hpp"
#include "test/support/NotificationTestHelper.hpp"

TEST(
    PublishPerformanceTest,
    MailboxSubscriberScaleWithRateLimit)
{
    constexpr std::size_t kSubscriberCounts[] =
    {
        10,
        100,
        1000,
        10000
    };

    for (const auto subscriberCount :
         kSubscriberCounts)
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
        rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager
        {
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext
        };

        for (std::size_t i = 0;
             i < subscriberCount;
             ++i)
        {
            subscriptionStore.Register(
            {
                static_cast<std::uint32_t>(i + 1),
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::DeliveryMethod::Mailbox,
                rim::NotificationTrigger::OnChange
            });
        }

        constexpr std::uint32_t
            kPublishes = 10000;

        const auto start =
            std::chrono::steady_clock::now();

        for (std::uint32_t i = 0;
             i < kPublishes;
             ++i)
        {
            publishManager.Publish(
                test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
                rim::NotificationTrigger::OnChange);
        }

        const auto end =
            std::chrono::steady_clock::now();

        const auto elapsedUs =
            std::chrono::duration_cast
            <
                std::chrono::microseconds
            >
            (
                end - start
            )
            .count();

        std::size_t totalNotifications =
            0;

        for (std::size_t i = 0;
             i < subscriberCount;
             ++i)
        {
            totalNotifications +=
                mailboxManager.GetMailbox(
                    static_cast<std::uint32_t>(
                        i + 1))
                .Count();
        }

        const auto expectedNotifications =
        subscriberCount;

        EXPECT_EQ(
            expectedNotifications,
            totalNotifications);

        const double publishPerSec =
            static_cast<double>(
                kPublishes)
            * 1000000.0
            / static_cast<double>(
                elapsedUs);

        const double deliveryPerSec =
            static_cast<double>(
                totalNotifications)
            * 1000000.0
            / static_cast<double>(
                elapsedUs);

        const double deliveryPerPublish =
            static_cast<double>(
                totalNotifications)
            / static_cast<double>(
                kPublishes);

        const double deliveryRatio =
            expectedNotifications > 0
            ? static_cast<double>(
                  totalNotifications)
                  * 100.0
                  / static_cast<double>(
                      expectedNotifications)
            : 0.0;

        std::cout
            << "\n=== PublishManager Mailbox Scale ===\n"

            << "Subscribers       : "
            << subscriberCount
            << '\n'

            << "Publishes         : "
            << kPublishes
            << '\n'

            << "ExpectedDelivery  : "
            << expectedNotifications
            << '\n'

            << "ActualDelivery    : "
            << totalNotifications
            << '\n'

            << "DeliveryRatio     : "
            << std::fixed
            << std::setprecision(2)
            << deliveryRatio
            << " %\n"

            << "Delivery/Publish  : "
            << deliveryPerPublish
            << '\n'

            << "Publish/sec       : "
            << static_cast<std::uint64_t>(
                   publishPerSec)
            << '\n'

            << "Delivery/sec      : "
            << static_cast<std::uint64_t>(
                   deliveryPerSec)
            << '\n'

            << "Elapsed(us)       : "
            << elapsedUs
            << '\n';
    }
}

TEST(
    PublishPerformanceTest,
    HeavyCallbackDoesNotBlockPublisher)
{
    constexpr int kCount = 100;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::CallbackQueue
        callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    std::atomic<int>
        received{0};

    const auto id =
        subscriptionStore
            .CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId id,
            const rim::NotificationMessage& message)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    10));

            ++received;
        });

    subscriptionStore.Register(
    {
        id,
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    callbackWorker.Run();

    auto publishStart =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kCount;
         ++i)
    {

        notifyManager.Notify(
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::NotificationTrigger::OnChange
        );
    }

    const auto publishTime =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::steady_clock::now()
                - publishStart);

    ASSERT_TRUE(
        WaitUntil(
            [&]
            {
                return received.load()
                    == kCount;
            },
            std::chrono::seconds(20)));

    callbackWorker.Stop();

    std::cout
        << "[PERF] publish(ms)="
        << publishTime.count()
        << std::endl;

    EXPECT_LT(
        publishTime.count(),
        100);
}