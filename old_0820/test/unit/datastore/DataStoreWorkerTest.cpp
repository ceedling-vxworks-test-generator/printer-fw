#include <gtest/gtest.h>

#include "printer_a.h"

#include "DataStoreWorker.hpp"

#include "BinaryStoreValue.hpp"
#include "IProductProvider.hpp"
#include "ProductFactory.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"

TEST(
    DataStoreWorkerTest,
    GenerateSnapshotEvent)
{
    rim::DomainStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        nullptr,
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::DataStoreWorker dispatcher(
        productProvider->GetProfile().definition,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        queues.capabilityQueue->TryPop(
            input));

    EXPECT_EQ(
        input.changedDataId,
        RI_DATA_UPPER_DOOR_OPEN);
}

TEST(
    DataStoreWorkerTest,
    UpdateDomainStorage)
{
    rim::DomainStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        nullptr,
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::DataStoreWorker dispatcher(
        productProvider->GetProfile().definition,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    const auto domainId =
        rim::DataDomainMap(
            productProvider->GetProfile().definition)
            .Find(
                RI_DATA_UPPER_DOOR_OPEN);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    auto* storage =
        domainStore.Find(
            domainId);

    ASSERT_NE(
        storage,
        nullptr);

    rim::RIMDataItem stored{};

    ASSERT_TRUE(
        storage->Find(
            RI_DATA_UPPER_DOOR_OPEN,
            stored));

    EXPECT_EQ(
        stored.value.type,
        rim::ValueType::kBool);
}

TEST(
    DataStoreWorkerTest,
    StoreErrorList)
{
    rim::DomainStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        nullptr,
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::DataStoreWorker dispatcher(
        productProvider->GetProfile().definition,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data =
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    // item.valueType =
    //     rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    const auto domainId =
        rim::DataDomainMap(
            productProvider->GetProfile().definition)
            .Find(
                RI_DATA_ERROR_LIST);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    auto* storage =
        domainStore.Find(
            domainId);

    ASSERT_NE(
        storage,
        nullptr);

    rim::RIMDataItem stored{};

    ASSERT_TRUE(
        storage->Find(
            RI_DATA_ERROR_LIST,
            stored));

    EXPECT_EQ(
        stored.value.type,
        rim::ValueType::kBinary);
}

TEST(
    DataStoreWorkerTest,
    StoreErrorListCopiesBinaryData)
{
    rim::DomainStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        nullptr,
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::DataStoreWorker dispatcher(
        productProvider->GetProfile().definition,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data =
    {
        0x11,
        0x22,
        0x33,
        0x44
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    // item.valueType =
    //     rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    const auto domainId =
        rim::DataDomainMap(
            productProvider->GetProfile().definition)
            .Find(
                RI_DATA_ERROR_LIST);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    auto* storage =
        domainStore.Find(
            domainId);

    ASSERT_NE(
        storage,
        nullptr);

    rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        storage->FindBinary(
            RI_DATA_ERROR_LIST,
            found));

    ASSERT_NE(
        found,
        nullptr);

    EXPECT_EQ(
        found->data.size(),
        4U);

    EXPECT_EQ(
        found->data[0],
        0x11);

    EXPECT_EQ(
        found->data[1],
        0x22);

    EXPECT_EQ(
        found->data[2],
        0x33);

    EXPECT_EQ(
        found->data[3],
        0x44);
}

TEST(
    DataStoreWorkerTest,
    NoNotificationWhenUnchanged)
{
    rim::DomainStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        nullptr,
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::DataStoreWorker dispatcher(
        productProvider->GetProfile().definition,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        queues.capabilityQueue->TryPop(
            input));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    EXPECT_FALSE(
        queues.capabilityQueue->TryPop(
            input));
}