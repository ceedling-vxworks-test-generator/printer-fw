#include <gtest/gtest.h>

#include "test/support/TestWaitHelper.hpp"

#include <atomic>

#include "AdapterDispatcher.hpp"
#include "products/printer_a/Adapter/PrinterAAdapter.hpp"

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

#include "NotificationMessage.hpp"
#include "NotificationTarget.hpp"
#include "NotificationTargetType.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

#include "PrinterAProductDefinition.hpp"
#include "RouteProvider.hpp"
#include "RouteExecutor.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"
#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"
#include "PrinterAProductDefinition.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"

TEST(
    AdapterToCallbackThreadTest,
    NotificationReceived)
{

    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::RouteProvider routeProvider;

    rim::PublisherInputQueue publisherQueue;

    rim::DomainStorageRegistry
        domainStore;

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

        rim::CallbackQueue callbackQueue; 
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

    routeProvider.Initialize(
    rim::kPrinterAProductDefinition);

    rim::PublishManager
        publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            routeProvider);

    auto productProvider = rim::CreatePrinterAProvider();

    routeProvider.Initialize(
        productProvider->GetProfile().definition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    std::vector<std::unique_ptr<rim::RouteExecutor>>
        routeExecutor;

    for (const auto& [name, queues] : routeProvider.GetQueues())
    {
        routeExecutor.push_back(
            std::make_unique<rim::RouteExecutor>(
                rim::kPrinterAProductDefinition,
                queues,
                domainStore,
                capabilityManager,
                publisherQueue,
                snapshotProvider));
    }

    rim::PublisherWorker publisherWorker(
        publisherQueue,
        publishManager);

    for (auto& route : routeExecutor)
    {
        route->Start();
    }

    callbackWorker.Run();
    publisherWorker.Run();

    rim::AdapterDispatcher dispatcher(
        rim::kPrinterAProductDefinition,
        routeProvider);

    rim::PrinterAAdapter adapter(
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
    callbackWorker.Stop();

    for (auto& route : routeExecutor)
    {
        route->Stop();
    }

}