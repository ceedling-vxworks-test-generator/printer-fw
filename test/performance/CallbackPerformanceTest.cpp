#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "PerformanceTestFixture.hpp"

#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"

#include "ChangeNotifyManager.hpp"

#include "test/support/TestWaitHelper.hpp"

namespace
{

constexpr int kMessageCount =
    10000;

std::atomic<int>
    g_receivedCount{0};

void OnEnvironment(
    uint64_t subscriptionId,
    const void* notificationMessage)
{
    (void)subscriptionId;
    (void)notificationMessage;
    ++g_receivedCount;
}

} // namespace

TEST_F(
    PerformanceTestFixture,
    EnvironmentCallbackThroughput)
{
    g_receivedCount = 0;

    uint64_t subscriptionId{};

    ASSERT_EQ(
        RIM_SubscribeCapability(
            RI_CAPABILITY_ENVIRONMENT,
            OnEnvironment,
            &subscriptionId),
        RI_SUCCESS);

    const auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kMessageCount;
         ++i)
    {
        ASSERT_EQ(
            RIM_SetDouble(
                RI_DATA_TEMPERATURE_SENSOR_A,
                static_cast<double>(i)),
            RI_SUCCESS);
    }

    std::this_thread::sleep_for(
        std::chrono::seconds(5));

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                end - start);

    const auto received =
        g_receivedCount.load();

    std::cout
        << "[PERF] Injected : "
        << kMessageCount
        << std::endl;

    std::cout
        << "[PERF] Received : "
        << received
        << std::endl;

    std::cout
        << "[PERF] Elapsed  : "
        << elapsedMs.count()
        << " ms"
        << std::endl;

    std::cout
        << "[PERF] Callback/sec : "
        << (1000.0 * received)
            / std::max(
                int64_t{1},
                elapsedMs.count())
        << std::endl;

    EXPECT_GT(
        received,
        0);

    ASSERT_EQ(
        RIM_Unsubscribe(
            subscriptionId),
        RI_SUCCESS);
}



#include <atomic>
#include <chrono>
#include <iostream>

#include "printer_a.h"

#include "CallbackSubscriptionRegistry.hpp"
#include "SubscriptionStore.hpp"

namespace
{

rim::NotificationTarget EnvironmentTarget()
{
    return
    {
        rim::NotificationTargetType::Capability,
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };
}

rim::NotificationMessage EnvironmentMessage()
{
    return
    {
        EnvironmentTarget(),
        rim::NotificationTrigger::OnChange
    };
}

}

TEST(
    CallbackPerformanceTest,
    CallbackInvokeThroughput)
{
    rim::CallbackSubscriptionRegistry
        registry;

    rim::SubscriptionStore
        store;

    std::atomic_uint64_t
        callbackCount{};

    const auto id =
        store.CreateSubscriptionId();

    registry.Subscribe(
        id,
        [&](rim::SubscriptionId,
            const rim::NotificationMessage&)
        {
            ++callbackCount;
        });

    constexpr std::uint32_t
        kIterations = 1000000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint32_t i = 0;
         i < kIterations;
         ++i)
    {
        registry.Notify(
            id,
            EnvironmentMessage());
    }

    const auto end =
        std::chrono::steady_clock::now();

    EXPECT_EQ(
        kIterations,
        callbackCount.load());

    const auto elapsedUs =
        std::chrono::duration_cast
        <
            std::chrono::microseconds
        >
        (
            end - start
        )
        .count();

    const auto throughput =
        static_cast<double>(
            kIterations)
        * 1000000.0
        / static_cast<double>(
            elapsedUs);

    std::cout
        << "\n=== Callback Throughput ===\n"
        << "Invocations : "
        << kIterations
        << '\n'
        << "Elapsed(us) : "
        << elapsedUs
        << '\n'
        << "Callback/sec: "
        << static_cast<std::uint64_t>(
               throughput)
        << '\n';
}

TEST(
    CallbackPerformanceTest,
    CallbackSubscriberScale)
{
    constexpr std::size_t kCounts[] =
    {
        10,
        100,
        1000,
        10000
    };

    for (const auto subscriberCount :
         kCounts)
    {
        rim::CallbackSubscriptionRegistry
            registry;

        rim::SubscriptionStore
            store;

        std::atomic_uint64_t
            callbackCount{};

        rim::SubscriptionId
            lastId{};

        for (std::size_t i = 0;
             i < subscriberCount;
             ++i)
        {
            lastId =
                store.CreateSubscriptionId();

            registry.Subscribe(
                lastId,
                [&](rim::SubscriptionId,
                    const rim::NotificationMessage&)
                {
                    ++callbackCount;
                });
        }

        constexpr std::uint32_t
            kIterations = 100000;

        const auto start =
            std::chrono::steady_clock::now();

        for (std::uint32_t i = 0;
             i < kIterations;
             ++i)
        {
            registry.Notify(
                lastId,
                EnvironmentMessage());
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

        const auto throughput =
            static_cast<double>(
                kIterations)
            * 1000000.0
            / static_cast<double>(
                elapsedUs);

        std::cout
            << "\n=== Callback Scale ===\n"
            << "Subscribers : "
            << subscriberCount
            << '\n'
            << "Callback/sec: "
            << static_cast<std::uint64_t>(
                   throughput)
            << '\n';
    }
}

TEST(
    CallbackPerformanceTest,
    CallbackLatency)
{
    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    std::atomic<bool> received{false};

    auto id =
        store.CreateSubscriptionId();

    std::chrono::microseconds latency{0};

    auto startTime =
        std::chrono::steady_clock::now();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId subscriptionId,
            const rim::NotificationMessage& message)
        {
            (void)subscriptionId;
            (void)message;

            latency =
                std::chrono::duration_cast<
                    std::chrono::microseconds>(
                        std::chrono::steady_clock::now()
                        - startTime);

            received = true;
        });

    store.Register(
    {
        id,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    callbackWorker.Run();

    manager.Notify(
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    },
    rim::NotificationTrigger::OnChange);

    ASSERT_TRUE(
        WaitUntil(
            [&]
            {
                return received.load();
            },
            std::chrono::seconds(1)));

    callbackWorker.Stop();

    std::cout
        << "[PERF] callback latency(us)="
        << latency.count()
        << std::endl;
}

TEST(
    CallbackPerformanceTest,
    QueueThroughput)
{
    constexpr int kCount = 10000;

    rim::SubscriptionStore store;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::CallbackQueue callbackQueue;

    rim::CallbackWorker callbackWorker(
        callbackQueue,
        callbackRegistry);

    rim::ChangeNotifyManager manager(
        store,
        mailboxManager,
        callbackRegistry,
        callbackQueue);

    std::atomic<int> received{0};

    auto id =
        store.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        id,
        [&](rim::SubscriptionId id,
            const rim::NotificationMessage& message)
        {
            ++received;
        });

    store.Register(
    {
        id,
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::DeliveryMethod::Callback,
        rim::NotificationTrigger::OnChange
    });

    callbackWorker.Run();

    auto start =
        std::chrono::steady_clock::now();

    for (int i = 0;
         i < kCount;
         ++i)
    {
        manager.Notify(
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::NotificationTrigger::OnChange);
    }

    ASSERT_TRUE(
        WaitUntil(
            [&]
            {
                return
                    received.load()
                    == kCount;
            },
            std::chrono::seconds(10)));

    const auto elapsed =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::steady_clock::now()
                - start);

    callbackWorker.Stop();

    std::cout
        << "[PERF] throughput="
        << (1000.0 * kCount)
               / elapsed.count()
        << " callbacks/sec"
        << std::endl;
}