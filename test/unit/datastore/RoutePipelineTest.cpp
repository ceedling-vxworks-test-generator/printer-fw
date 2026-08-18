#include <gtest/gtest.h>

#include "printer_a.h"

#include "RouteProvider.hpp"
#include "RoutePipeline.hpp"

#include "DataDomainMap.hpp"
#include "RIMSnapshotManager.hpp"
#include "DomainStorageRegistry.hpp"

#include "CapabilityStore.hpp"
#include "CapabilityManager.hpp"

#include "PublisherInputQueue.hpp"

#include "RIMValueFactory.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

TEST(
    RoutePipelineTest,
    RoutePipelineCountMatchesRouteCount)
{
    auto productProvider =
        rim::CreatePrinterAProvider();

    const auto& product =
        productProvider->GetProfile().definition;

    rim::RouteProvider routeProvider;

    routeProvider.Initialize(
        product);

    rim::DomainStorageRegistry domainStore;

    rim::RIMSnapshotManager Reader(
        domainStore);

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    std::vector<
        std::unique_ptr<rim::RoutePipeline>>
        pipelines;

    for (const auto& [name, queues]: routeProvider.GetQueues())
    {
        pipelines.push_back(
            std::make_unique<rim::RoutePipeline>(
                product,
                queues,
                domainStore,
                Reader,
                capabilityManager,
                publisherQueue));
    }

    EXPECT_EQ(
        pipelines.size(),
        routeProvider.GetQueues().size());
}

TEST(
    RoutePipelineTest,
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
    RoutePipelineTest,
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

    rim::RIMSnapshotManager Reader(
        domainStore);

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    auto queueIt =
        routeProvider.GetQueues().find(
            "ValueRoute");

    ASSERT_NE(
        queueIt,
        routeProvider.GetQueues().end());

    rim::RoutePipeline pipeline(
        product,
        queueIt->second,
        domainStore,
        Reader,
        capabilityManager,
        publisherQueue);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queueIt->second.storeQueue->Push(
            item));

    ASSERT_TRUE(
        pipeline.ExecuteOnce());

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