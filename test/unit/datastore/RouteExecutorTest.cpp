#include <gtest/gtest.h>

#include "printer_a.h"

#include "RouteProvider.hpp"
#include "RouteExecutor.hpp"

#include "DataDomainMap.hpp"
#include "DomainStorageRegistry.hpp"

#include "CapabilityStore.hpp"
#include "CapabilityManager.hpp"

#include "PublisherInputQueue.hpp"

#include "RIMValueFactory.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"

TEST(
    RouteExecutorTest,
    RouteExecutorCountMatchesRouteCount)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        product);

    rim::DomainStorageRegistry domainStore;

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            product);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            product);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    std::vector<
        std::unique_ptr<rim::RouteExecutor>>
        routeExecutor;

    for (const auto& [name, queues] : routeProvider.GetQueues())
    {
        routeExecutor.push_back(
            std::make_unique<rim::RouteExecutor>(
                product,
                queues,
                domainStore,
                capabilityManager,
                publisherQueue,
                snapshotProvider));
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

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        product);

    EXPECT_EQ(
        routeProvider.GetQueues().size(),
        GetRouteCount(product));
}

TEST(
    RouteExecutorTest,
    ProcessItemFromRouteQueue)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        product);

    rim::DomainStorageRegistry domainStore;

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            product);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            product);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    auto queueIt =
        routeProvider.GetQueues().find(
            "ValueRoute");

    ASSERT_NE(
        queueIt,
        routeProvider.GetQueues().end());

    rim::RouteExecutor routeExecutor(
        product,
        queueIt->second,
        domainStore,
        capabilityManager,
        publisherQueue,
        snapshotProvider);

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
        routeExecutor.ExecuteOnce());

    rim::DataDomainMap dataDomainMap(
        product);

    const auto domainId =
        dataDomainMap.Find(
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