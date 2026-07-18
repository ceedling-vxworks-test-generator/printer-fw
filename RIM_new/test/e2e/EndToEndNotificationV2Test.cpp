#include <gtest/gtest.h>

#include "adapter/AdapterDispatcher.hpp"
#include "adapter/PrinterAdapter.hpp"

#include "datastore/StoreInputQueue.hpp"
#include "datastore/DataStoreWorker.hpp"
#include "datastore/AggregateRIMSnapshotReader.hpp"

#include "capability/CapabilityInputQueue.hpp"
#include "capability/CapabilityWorker.hpp"
#include "capability/CapabilityManager.hpp"
#include "capability/MachineCapabilityStore.hpp"

#include "capability/PrinterACapabilityRuleSet.hpp"

#include "publisher/PublisherInputQueue.hpp"
#include "publisher/PublisherWorker.hpp"

#include "publisher/PublishManager.hpp"
#include "publisher/ChangeNotifyManager.hpp"

#include "publisher/SubscriptionRegistry.hpp"
#include "publisher/SubscriberMailbox.hpp"
#include "publisher/CallbackSubscriptionRegistry.hpp"

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

    rim::PrinterACapabilityRuleSet rules;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        rules);

    rim::SubscriptionRegistry registry;

    registry.Subscribe(
        1);

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry callbackRegistry;

    bool called = false;

    callbackRegistry.SubscribeEnvironment(
        [&](const rim::EnvironmentCapability& capability)
        {
            called = true;
        });

    rim::ChangeNotifyManager notifyManager(
        registry,
        mailbox,
        callbackRegistry);

    rim::PublishManager publishManager(
        capabilityStore,
        notifyManager);

    rim::DataStoreWorker dataStoreWorker(
        storeQueue,
        valueStore,
        reader,
        capabilityQueue);

    rim::CapabilityWorker capabilityWorker(
        capabilityQueue,
        capabilityManager,
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