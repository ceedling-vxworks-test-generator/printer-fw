#include <gtest/gtest.h>
#include <atomic>

#include "test/support/TestWaitHelper.hpp"

#include "AdapterDispatcher.hpp"
#include "products/printer_a/Adapter/PrinterAAdapter.hpp"

#include "PartitionStorageRegistry.hpp"

#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"

#include "FacadeManager.hpp"

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
#include "RouteExecutor.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

#include "CallbackQueue.hpp"
#include "CallbackWorker.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"
#include "FacadeSnapshotProvider.hpp"

#include "test/support/NotificationTestHelper.hpp"
#include "ProductContext.hpp"

TEST(
    EndToEndNotificationV2Test,
    AdapterToCallback)
{
    rim::PublisherInputQueue publisherQueue;

    rim::PartitionStorageRegistry domainStore;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        productContext);

    rim::CapabilityManager capabilityManager(
        domainStore,
        productContext);

    rim::FacadeManager facadeManager(
        domainStore,
        productContext);

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
        test::CapabilityTarget(
            RI_CAPABILITY_ENVIRONMENT);

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
        publishManager{
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            productContext};

    rim::CapabilitySnapshotResolver
        capabilitySnapshotResolver(
            productContext);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            productContext);

    rim::CapabilitySnapshotProvider
        capabilitySnapshotProvider(
            capabilitySnapshotResolver,
            snapshotAccessor);

    rim::FacadeSnapshotProvider
        facadeSnapshotProvider(
            productContext,
            domainStore);

    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    std::vector<
        std::unique_ptr<
            rim::RouteExecutor>>
        routeExecutor;

    for (const auto& [name, queues]
         : routeProvider.GetQueues())
    {
        routeExecutor.push_back(
            std::make_unique<
                rim::RouteExecutor>(
                    productContext,
                    queues,
                    domainStore,
                    capabilityManager,
                    facadeManager,
                    publisherQueue,
                    capabilitySnapshotProvider,
                    facadeSnapshotProvider));
    }

    rim::PublisherWorker
        publisherWorker(
            publisherQueue,
            publishManager);

    rim::AdapterDispatcher
        dispatcher(
            productContext,
            routeProvider);

    rim::PrinterAAdapter
        adapter(
            dispatcher);

    callbackWorker.Run();

    ASSERT_TRUE(
        adapter.Poll());

    for (auto& route : routeExecutor)
    {
        route->ExecuteOnce();
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