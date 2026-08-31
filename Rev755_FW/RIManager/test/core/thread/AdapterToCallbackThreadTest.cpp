#include <gtest/gtest.h>

#include "test/core/support/TestWaitHelper.hpp"
#include "test/core/support/TestDomainInitializer.hpp"

#include <atomic>

#include "AdapterDispatcher.hpp"
#include "Adapter/Adapter.hpp"

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

#include "NotificationMessage.hpp"
#include "RIMId.hpp"
#include "NotificationTrigger.hpp"
#include "SubscriptionInfo.hpp"
#include "DeliveryMethod.hpp"

#include "Product.hpp"
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

#include "test/core/support/NotificationTestHelper.hpp"
#include "ProductContext.hpp"

TEST(
    AdapterToCallbackThreadTest,
    NotificationReceived)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::RouteProvider
        routeProvider;

    rim::PublisherInputQueue
        publisherQueue;

    rim::PartitionStorageRegistry
        domainStore;

    rim::ProductContext
        productContext(
            rim::kProductDefinition);

    rim::test::RegisterAllDomains(
        domainStore,
        productContext);

    routeProvider.Initialize(
        productContext);

    rim::CapabilityManager
        capabilityManager(
            domainStore,
            productContext);

    rim::FacadeManager
        facadeManager(
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
            snapshotAccessor);

    auto productProvider =
        rim::CreateProvider();

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

    for (auto& route : routeExecutor)
    {
        route->Start();
    }

    callbackWorker.Run();
    publisherWorker.Run();

    rim::AdapterDispatcher
        dispatcher(
            productContext,
            routeProvider);

    rim::Adapter
        adapter(
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