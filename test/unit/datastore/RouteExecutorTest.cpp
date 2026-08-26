#include <gtest/gtest.h>

#include "printer_a.h"

#include "RouteProvider.hpp"
#include "RouteExecutor.hpp"

#include "DataDomainMap.hpp"
#include "PartitionStorageRegistry.hpp"

#include "CapabilityManager.hpp"
#include "FacadeManager.hpp"

#include "PublisherInputQueue.hpp"

#include "RIMValueFactory.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"
#include "FacadeSnapshotProvider.hpp"
#include "ProductContext.hpp"

TEST(
    RouteExecutorTest,
    RouteExecutorCountMatchesRouteCount)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::ProductContext productContext(
        product);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        productContext);

    rim::PartitionStorageRegistry domainStore;

    rim::CapabilityManager capabilityManager(
        domainStore,
        productContext);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            productContext);

    rim::CapabilitySnapshotProvider
        capabilitySnapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::FacadeManager facadeManager(
        domainStore,
        productContext);

    rim::FacadeSnapshotProvider
        facadeSnapshotProvider(
            productContext,
            domainStore);
            
    std::vector<
        std::unique_ptr<rim::RouteExecutor>>
        routeExecutor;

    for (const auto& [name, queues] : routeProvider.GetQueues())
    {
        routeExecutor.push_back(
        std::make_unique<rim::RouteExecutor>(
            productContext,
            queues,
            domainStore,
            capabilityManager,
            facadeManager,
            publisherQueue,
            capabilitySnapshotProvider,
            facadeSnapshotProvider));
    }

    EXPECT_EQ(
        routeExecutor.size(),
        routeProvider.GetQueues().size());
}

TEST(
    RouteExecutorTest,
    RouteProviderCreatesQueueForEveryRouteDefinition)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::ProductContext productContext(
        product);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        productContext);

    EXPECT_EQ(
        routeProvider.GetQueues().size(),
        productContext.GetRouteCount());
}

TEST(
    RouteExecutorTest,
    ProcessItemFromRouteQueue)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::ProductContext productContext(
        product);

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        productContext);

    rim::PartitionStorageRegistry domainStore;

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            productContext);

    rim::CapabilityManager capabilityManager(
        domainStore,
        productContext);

    rim::CapabilitySnapshotProvider
        capabilitySnapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::FacadeManager facadeManager(
        domainStore,
        productContext);

    rim::FacadeSnapshotProvider
        facadeSnapshotProvider(
            productContext,
            domainStore);

    auto queueIt =
        routeProvider.GetQueues().find(
            "ValueRoute");

    ASSERT_NE(
        queueIt,
        routeProvider.GetQueues().end());

    auto routeExecutor =
        std::make_unique<rim::RouteExecutor>(
            productContext,
            queueIt->second,
            domainStore,
            capabilityManager,
            facadeManager,
            publisherQueue,
            capabilitySnapshotProvider,
            facadeSnapshotProvider);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queueIt->second.storeQueue->Push(
            item));

    ASSERT_TRUE(
        routeExecutor->ExecuteOnce());

    const auto domainId =
        productContext
            .FindDataDomainId(
                RI_DATA_UPPER_DOOR_OPEN);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    const auto* storage =
        domainStore.Find(
            domainId);

    ASSERT_NE(
        storage,
        nullptr);

    rim::RIMDataItem stored{};

    EXPECT_TRUE(
        storage->Find(
            RI_DATA_UPPER_DOOR_OPEN,
            stored));

    EXPECT_EQ(
        stored.value.type,
        rim::ValueType::kBool);
}