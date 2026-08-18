#include <gtest/gtest.h>

#include "DataStoreWorker.hpp"
#include "RIMSnapshotManager.hpp"
#include "BinaryStoreValue.hpp"
#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"

#include "PrinterAProductDefinition.hpp"
#include "RouteProvider.hpp"
#include "RoutePipeline.hpp"

#include "ProductFactory.hpp"
#include "IProductProvider.hpp"

TEST(
    DataStoreWorkerTest,
    ExecuteOnce)
{
    rim::StoreInputQueue queue;

    rim::DomainStorageRegistry domainStore;

    rim::RIMSnapshotManager reader(
        domainStore);

    rim::RouteProvider routeProvider;

    auto productProvider = rim::CreatePrinterAProvider();

    routeProvider.Initialize(productProvider->GetProfile().definition);

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        domainStore,
        reader,
        routeProvider);

    rim::RIMDataItem item{};

    item.id = RI_DATA_UPPER_DOOR_OPEN;
    item.valueType = rim::ValueType::kBool;
    item.value = rim::RIMValueFactory::CreateBool(true);
    queue.Push(
        item);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    DataStoreWorkerTest,
    StoreErrorListKeepsOriginalData)
{
    rim::StoreInputQueue queue;

    rim::DomainStorageRegistry domainStore;

    rim::RIMSnapshotManager reader(
        domainStore);

    rim::RouteProvider routeProvider;

    auto productProvider = rim::CreatePrinterAProvider();

    routeProvider.Initialize(productProvider->GetProfile().definition);

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        domainStore,
        reader,
        routeProvider);

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data =
    {
        1,
        2,
        3,
        4
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.get());

    auto original =
        binary->data;

    queue.Push(
        item);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    ASSERT_EQ(
        binary->data,
        original);
}