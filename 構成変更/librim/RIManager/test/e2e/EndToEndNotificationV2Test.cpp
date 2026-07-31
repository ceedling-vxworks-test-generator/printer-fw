#include <gtest/gtest.h>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"

#include "AggregateRIMSnapshotReader.hpp"
#include "DataStoreWorker.hpp"
#include "ErrorRepository.hpp"
#include "StoreInputQueue.hpp"

#include "CapabilityEvaluator.hpp"
#include "CapabilityInputQueue.hpp"
#include "CapabilityStore.hpp"
#include "CapabilityWorker.hpp"

#include "CallbackSubscriptionRegistry.hpp"
#include "CapabilityPublisherRegistry.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"
#include "PublishManager.hpp"
#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"
#include "SubscriberMailbox.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"

//
// Adapter からコールバック到達までの全段。
//
// Product(PrinterACapabilityRuleSet)が規則を持ち込み、Core の各段は
// CapabilityId と型消去バイト列だけを扱う、という現在の構成をそのまま組んでいる。
//

TEST(
    EndToEndNotificationV2Test,
    AdapterToCallback)
{
    rim::StoreInputQueue      storeQueue;
    rim::CapabilityInputQueue capabilityQueue;
    rim::PublisherInputQueue  publisherQueue;

    rim::ValueStore valueStore;

    rim::AggregateRIMSnapshotReader reader(
        valueStore);

    rim::CapabilityStore capabilityStore;

    rim::ErrorRepository errorRepository;

    rim::PrinterACapabilityRuleSet ruleSet(
        errorRepository);

    rim::CapabilityEvaluator capabilityEvaluator;

    ruleSet.RegisterTo(
        capabilityEvaluator);

    rim::SubscriberMailbox mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    bool called = false;

    callbackRegistry.Subscribe(
        rim::kCapEnvironment,
        [&](rim::SubscriptionId,
            rim::CapabilityId,
            const rim::CapabilityPayload&)
        {
            called = true;
        });

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::GenericCapabilityPublisher publisher(
        [&]
        {
            rim::CapabilityPayload payload;

            if (capabilityStore.TryGet(
                    rim::kCapEnvironment,
                    payload))
            {
                notifyManager.Notify(
                    rim::kCapEnvironment,
                    payload);
            }
        });

    publisherRegistry.Register(
        rim::kCapEnvironment,
        &publisher);

    rim::PublishManager publishManager(
        publisherRegistry);

    rim::DataStoreWorker dataStoreWorker(
        storeQueue,
        valueStore,
        reader,
        capabilityQueue);

    rim::CapabilityWorker capabilityWorker(
        capabilityQueue,
        capabilityEvaluator,
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
