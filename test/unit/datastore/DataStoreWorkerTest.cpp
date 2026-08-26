#include <gtest/gtest.h>

#include "printer_a.h"

#include "DataStoreWorker.hpp"

#include "IProductProvider.hpp"
#include "ProductFactory.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"
#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"

TEST(
    DataStoreWorkerTest,
    GenerateSnapshotEvent)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dispatcher(
        productContext,
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
    UpdatePartitionStorage)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dispatcher(
        productContext,
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
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dispatcher(
        productContext,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            data,
            sizeof(data));

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
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dispatcher(
        productContext,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33,
        0x44
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            data,
            sizeof(data));

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

    const std::uint8_t* bytes{};
    std::size_t size{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBinary(
            stored.value,
            bytes,
            size));

    EXPECT_EQ(size, 4U);

    EXPECT_EQ(bytes[0], 0x11);
    EXPECT_EQ(bytes[1], 0x22);
    EXPECT_EQ(bytes[2], 0x33);
    EXPECT_EQ(bytes[3], 0x44);
}

TEST(
    DataStoreWorkerTest,
    NoNotificationWhenUnchanged)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };


    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker dispatcher(
        productContext,
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

TEST(
    DataStoreWorkerTest,
    SameBinaryDoesNotNotifyAgain)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker worker(
        productContext,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            data,
            sizeof(data));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        queues.capabilityQueue->TryPop(
            input));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_FALSE(
        queues.capabilityQueue->TryPop(
            input));
}

TEST(
    DataStoreWorkerTest,
    BinaryHashChangedNotifiesAgain)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker worker(
        productContext,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    const std::uint8_t data1[]
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item1{};

    item1.id =
        RI_DATA_ERROR_LIST;

    item1.value =
        rim::RIMValueFactory::CreateBinary(
            data1,
            sizeof(data1));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item1));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        queues.capabilityQueue->TryPop(
            input));

    const std::uint8_t data2[]
    {
        0x11,
        0x22,
        0x44
    };

    rim::RIMDataItem item2{};

    item2.id =
        RI_DATA_ERROR_LIST;

    item2.value =
        rim::RIMValueFactory::CreateBinary(
            data2,
            sizeof(data2));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item2));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_TRUE(
        queues.capabilityQueue->TryPop(
            input));
}

TEST(
    DataStoreWorkerTest,
    BinarySizeChangedNotifiesAgain)
{
    rim::PartitionStorageRegistry domainStore;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::RouteQueues queues
    {
        rim::QueueFactory::Create<rim::RIMDataItem>(
            rim::QueuePolicy::Buffered),

        rim::QueueFactory::Create<rim::CapabilityInput>(
            rim::QueuePolicy::Buffered)
    };

    rim::ProductContext productContext(
        productProvider->GetProfile().definition);

    rim::DataStoreWorker worker(
        productContext,
        *queues.storeQueue,
        domainStore,
        *queues.capabilityQueue);

    const std::uint8_t data1[]
    {
        0x11,
        0x22,
        0x33
    };

    rim::RIMDataItem item1{};

    item1.id =
        RI_DATA_ERROR_LIST;

    item1.value =
        rim::RIMValueFactory::CreateBinary(
            data1,
            sizeof(data1));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item1));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        queues.capabilityQueue->TryPop(
            input));

    const std::uint8_t data2[]
    {
        0x11,
        0x22,
        0x33,
        0x44
    };

    rim::RIMDataItem item2{};

    item2.id =
        RI_DATA_ERROR_LIST;

    item2.value =
        rim::RIMValueFactory::CreateBinary(
            data2,
            sizeof(data2));

    ASSERT_TRUE(
        queues.storeQueue->Push(
            item2));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    EXPECT_TRUE(
        queues.capabilityQueue->TryPop(
            input));
}