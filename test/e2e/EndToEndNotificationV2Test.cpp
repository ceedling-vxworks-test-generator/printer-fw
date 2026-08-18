#include <gtest/gtest.h>
#include <atomic>

#include "test/support/TestWaitHelper.hpp"

#include "AdapterDispatcher.hpp"
#include "products/printer_a/Adapter/PrinterAAdapter.hpp"

#include "StoreInputQueue.hpp"
#include "DataStoreWorker.hpp"
#include "RIMSnapshotManager.hpp"
#include "DomainStorageRegistry.hpp"

#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"
#include "CapabilityStore.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"

#include "PublishManager.hpp"
#include "ChangeNotifyManager.hpp"
#include "PeriodicNotifyManager.hpp"

#include "SubscriptionStore.hpp"
#include "SubscriberMailboxManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"

#include "NotificationTarget.hpp"
#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

#include "PrinterAProductDefinition.hpp"
#include "RouteProvider.hpp"
#include "RoutePipeline.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"

TEST(
    EndToEndNotificationV2Test,
    AdapterToCallback)
{
    rim::StoreInputQueue storeQueue;

    rim::RouteProvider routeProvider;

    rim::PublisherInputQueue publisherQueue;

    rim::DomainStorageRegistry domainStore;

    rim::RIMSnapshotManager reader(
        domainStore);

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        rim::kPrinterAProductDefinition);

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    std::atomic<bool>
        called{false};

    const rim::SubscriptionId
        subscriptionId =
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

    rim::CallbackQueue
        callbackQueue;

    rim::CallbackWorker
        callbackWorker(
            callbackQueue,
            callbackRegistry);

    rim::ChangeNotifyManager
        notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue);

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition);

    auto productProvider =
        rim::CreatePrinterAProvider();

    routeProvider.Initialize(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dataStoreWorker(
        rim::kPrinterAProductDefinition,
        storeQueue,
        domainStore,
        reader,
        routeProvider);

    std::vector<std::unique_ptr<rim::RoutePipeline>> pipelines;

    for (const auto& [name, queues]: routeProvider.GetQueues())
    {
        pipelines.push_back(
            std::make_unique<rim::RoutePipeline>(
                rim::kPrinterAProductDefinition,
                queues,
                domainStore,
                reader,
                capabilityManager,
                publisherQueue));
    }

    rim::PublisherWorker
        publisherWorker(
            publisherQueue,
            publishManager);

    rim::AdapterDispatcher
        dispatcher(
            rim::kPrinterAProductDefinition,
            storeQueue);

    rim::PrinterAAdapter
        adapter(
            dispatcher);

    callbackWorker.Run();

    ASSERT_TRUE(
        adapter.Poll());

    ASSERT_TRUE(
        dataStoreWorker.ExecuteOnce());

    for (auto& pipeline : pipelines)
    {
        pipeline->ExecuteOnce();
    }

    EXPECT_TRUE(
        WaitUntil(
            [&]
            {
                publisherWorker.ExecuteOnce();

                return called.load();
            },
            std::chrono::milliseconds(
                100)));

    callbackWorker.Stop();

    EXPECT_TRUE(
        called.load());
}