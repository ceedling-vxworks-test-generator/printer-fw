#include <gtest/gtest.h>

#include "RouteProvider.hpp"
#include "RoutePipeline.hpp"

#include "ValueStore.hpp"
#include "RIMSnapshotManager.hpp"

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

    rim::ValueStore valueStore;

    rim::RIMSnapshotManager Reader(valueStore);

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    std::vector<
        std::unique_ptr<rim::RoutePipeline>>
        pipelines;

    for (const auto& route :
         routeProvider.GetQueues())
    {
        pipelines.push_back(
            std::make_unique<
                rim::RoutePipeline>(
                    product,
                    *route.second,
                    valueStore,
                    productProvider->GetChangeChecker(),
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

    rim::ValueStore valueStore;

    rim::RIMSnapshotManager Reader(valueStore);

    rim::CapabilityStore capabilityStore;

    rim::CapabilityManager capabilityManager(
        capabilityStore,
        product);

    rim::PublisherInputQueue publisherQueue;

    auto queueIt =
        routeProvider.GetQueues().find(
            "Value");

    ASSERT_NE(
        queueIt,
        routeProvider.GetQueues().end());

    rim::RoutePipeline pipeline(
        product,
        *queueIt->second,
        valueStore,
        productProvider->GetChangeChecker(),
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
        queueIt->second->push(
            item));

    ASSERT_TRUE(
        pipeline.ExecuteOnce());

    rim::RIMDataItem stored{};

    EXPECT_TRUE(
        valueStore.Find(
            RI_DATA_UPPER_DOOR_OPEN,
            stored));
}