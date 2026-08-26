#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <mutex>
#include <set>
#include <thread>

#include "CallbackWorker.hpp"
#include "CallbackQueue.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "CallbackNotification.hpp"

#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"

#include "test/support/TestWaitHelper.hpp"
#include "test/support/NotificationTestHelper.hpp"
#include "NotificationMessage.hpp"
#include "printer_a.h"

TEST(
    CallbackWorkerTest,
    MultipleWorkersCanProcessCallbacks)
{
    rim::CallbackQueue queue;

    rim::CallbackSubscriptionRegistry registry;

    std::atomic<int> callbackCount{0};

    std::mutex mutex;

    std::set<std::thread::id>
        threadIds;

    constexpr rim::SubscriptionId
        subscriptionId = 1;

    registry.Subscribe(
        subscriptionId,
        [&](rim::SubscriptionId id,
            const rim::NotificationMessage& message)
        {
            {
                std::lock_guard<std::mutex>
                    lock(mutex);

                threadIds.insert(
                    std::this_thread::get_id());
            }

            ++callbackCount;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(20));
        });

    rim::CallbackWorker worker(
        queue,
        registry);

    worker.Run(4);

    for (int i = 0; i < 20; ++i)
    {
        rim::CallbackNotification
            notification{};

        notification.subscriptionId =
            subscriptionId;

        notification.target =
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY);

        notification.trigger =
            rim::NotificationTrigger::OnChange;

        queue.Push(
            notification);
    }

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return
                    callbackCount.load()
                    == 20;
            },
            std::chrono::seconds(2)));

    worker.Stop();

    EXPECT_EQ(
        callbackCount.load(),
        20);

    EXPECT_GT(
        threadIds.size(),
        1u);
}