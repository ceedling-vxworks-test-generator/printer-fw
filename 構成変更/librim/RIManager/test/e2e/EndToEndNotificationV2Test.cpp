#include <gtest/gtest.h>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"

#include "StoreInputQueue.hpp"
#include "DataStoreWorker.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "ErrorRepository.hpp"

#include "CapabilityInputQueue.hpp"
#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"

#include "SubscriptionRegistry.hpp"
#include "SubscriberMailbox.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

TEST(
    EndToEndNotificationV2Test,
    AdapterToCallback)
{
    rim::StoreInputQueue storeQueue;

    rim::CapabilityInputQueue capabilityQueue;

    rim::PublisherInputQueue publisherQueue;

    rim::ValueStore valueStore;

    rim::AggregateRIMSnapshotReader reader(
        valueStore);

    rim::MachineCapabilityStore capabilityStore;

    rim::ErrorRepository errorRepository;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        errorRepository);

    rim::SubscriptionRegistry
        subscriptionRegistry;

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.SubscribeEnvironment(
        [&](rim::SubscriptionId subscriptionId,
            const rim::EnvironmentCapability& capability)
        {
            (void)subscriptionId;
            (void)capability;

            called = true;
        });

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    publisherRegistry.Register(
        "Environment",
        std::make_unique<
            rim::GenericCapabilityPublisher>(
            [&]
            {
                notifyManager.Notify(
                    capabilityStore.GetEnvironment());
            }));

    rim::PublishManager
        publishManager(
            publisherRegistry);

    rim::DataStoreWorker dataStoreWorker(
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

    rim::AdapterDispatcher dispatcher(
        storeQueue);

    rim::PrinterAdapter adapter(
        dispatcher);

    ASSERT_TRUE(
        adapter.Poll());

    ASSERT_TRUE(
        dataStoreWorker.ExecuteOnce());

    ASSERT_TRUE(
        capabilityWorker.ExecuteOnce());

    ASSERT_TRUE(
        publisherWorker.ExecuteOnce());

    EXPECT_TRUE(
        called);
}