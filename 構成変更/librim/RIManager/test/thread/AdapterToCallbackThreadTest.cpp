#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "AdapterDispatcher.hpp"
#include "PrinterAdapter.hpp"

#include "StoreInputQueue.hpp"
#include "DataStoreWorker.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "ErrorRepository.hpp"

#include "CapabilityEvaluator.hpp"
#include "CapabilityInputQueue.hpp"
#include "CapabilityStore.hpp"
#include "CapabilityWorker.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"

#include "SubscriberMailbox.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "CapabilityPublisherRegistry.hpp"
#include "GenericCapabilityPublisher.hpp"

#include "test/support/CallbackTestHelper.hpp"

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityRuleSet.hpp"

//
// ワーカを実際にスレッドで回して、Adapter の1回のポーリングが
// コールバックまで到達することを確認する。
//
// Product が規則を持ち込み、Core の各段は CapabilityId と型消去バイト列だけを
// 扱う、という現在の構成をそのまま組んでいる。
//

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

    rim::CapabilityRecorder recorder;

    callbackRegistry.Subscribe(
        rim::kCapEnvironment,
        rim::CapabilityRecorder::Callback,
        &recorder);

    rim::ChangeNotifyManager notifyManager(
        mailbox,
        callbackRegistry);

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    rim::StorePublishBinding binding
    {
        &capabilityStore,
        &notifyManager,
        rim::kCapEnvironment
    };

    rim::GenericCapabilityPublisher publisher(
        rim::StorePublishBinding::Publish,
        &binding);

    publisherRegistry.Register(
        rim::kCapEnvironment,
        &publisher);

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
        capabilityEvaluator,
        capabilityStore,
        publisherQueue);

    rim::PublisherWorker publisherWorker(
        publisherQueue,
        publishManager);

    dataStoreWorker.Run();

    capabilityWorker.Run();

    publisherWorker.Run();

    rim::AdapterDispatcher dispatcher(
        storeQueue);

    rim::PrinterAdapter adapter(
        dispatcher);

    ASSERT_TRUE(
        adapter.Poll());

    const auto timeout =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(1);

    while (!recorder.Called() &&
           std::chrono::steady_clock::now() <
               timeout)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(10));
    }

    publisherWorker.Stop();

    capabilityWorker.Stop();

    dataStoreWorker.Stop();

    EXPECT_TRUE(
        recorder.Called());
}