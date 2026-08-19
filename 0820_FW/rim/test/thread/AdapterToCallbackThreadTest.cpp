#include <gtest/gtest.h>

#include "test/support/TestWaitHelper.hpp"

#include <atomic>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"

#include "StoreInputQueue.hpp"
#include "DataStoreWorker.hpp"
#include "AggregateRIMSnapshotReader.hpp"

#include "CapabilityInputQueue.hpp"
#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "NotificationMessage.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

#include "PrinterAProductDefinition.hpp"

TEST(
    AdapterToCallbackThreadTest,
    NotificationReceived)
{
    rim::StoreInputQueue storeQueue;

    rim::CapabilityInputQueue capabilityQueue;

    rim::PublisherInputQueue publisherQueue;

    rim::ValueStore valueStore;

    rim::AggregateRIMSnapshotReader reader(
        valueStore);

    rim::MachineCapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        rim::kPrinterAProductDefinition);

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    std::atomic<bool> called{
        false};

    const auto subscriptionId =
        subscriptionStore.CreateSubscriptionId();

    callbackRegistry.Subscribe(
        subscriptionId,
        [&](rim::SubscriptionId id,
            const rim::NotificationMessage& message)
        {
            (void)id;
            (void)message;

            called = true;
        });

    rim::SubscriptionInfo info{};

    info.id =
        subscriptionId;

    info.target =
    {
        rim::NotificationTargetType::Capability,

        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT)
    };

    info.method =
        rim::DeliveryMethod::Callback;

    info.trigger =
        rim::NotificationTrigger::OnChange;

    subscriptionStore.Register(
        info);

    rim::ChangeNotifyManager notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager publishManager(
        notifyManager,
        periodicNotifyManager,
        subscriptionStore);

    rim::DataStoreWorker dataStoreWorker(
        rim::kPrinterAProductDefinition,
        storeQueue,
        valueStore,
        reader,
        capabilityQueue);

    rim::CapabilityWorker capabilityWorker(
        capabilityQueue,
        capabilityManager,
        capabilityStore,
        publisherQueue);

    rim::PublisherWorker publisherWorker(
        publisherQueue,
        publishManager);

    dataStoreWorker.Run();

    capabilityWorker.Run();

    publisherWorker.Run();

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProductDefinition,
        storeQueue);

    rim::PrinterAdapter adapter(
        dispatcher);

    ASSERT_TRUE(
        adapter.Poll());

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                return called.load();
            },
            std::chrono::seconds(1)));

    publisherWorker.Stop();

    capabilityWorker.Stop();

    dataStoreWorker.Stop();
}