#include <gtest/gtest.h>

#include "rim_api.h"

#include "test/core/support/TestDomainInitializer.hpp"

#include "DataStoreWorker.hpp"

#include "IProductProvider.hpp"
#include "ProductFactory.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"
#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "RouteProvider.hpp"
#include "QueueFactory.hpp"

namespace
{

class DataStoreWorkerFixture
    : public ::testing::Test
{
protected:

    rim::PartitionStorageRegistry
        domainStore;

    std::unique_ptr<rim::IProductProvider>
        productProvider;

    rim::ProductContext
        productContext;

    rim::RouteQueues
        queues;

    DataStoreWorkerFixture()
        :
        productProvider(
            rim::CreateProvider()),

        productContext(
            productProvider->GetProfile().definition),

        queues
        {
            rim::QueueFactory::Create<rim::RIMDataItem>(
                rim::QueuePolicy::Buffered),

            rim::QueueFactory::Create<rim::CapabilityInput>(
                rim::QueuePolicy::Buffered)
        }
    {
        rim::test::RegisterAllDomains(
            domainStore,
            productContext);
    }
};

}

TEST_F(
    DataStoreWorkerFixture,
    GenerateSnapshotEvent)
{
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
        input.changedId.type,
        rim::RIMIdType::Data);

    EXPECT_EQ(
        input.changedId.id,
        RI_DATA_UPPER_DOOR_OPEN);

}

TEST_F(
    DataStoreWorkerFixture,
    UpdatePartitionStorage)
{
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
                {
                    rim::RIMIdType::Data,
                    RI_DATA_UPPER_DOOR_OPEN
                });

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

TEST_F(
    DataStoreWorkerFixture,
    StoreErrorList)
{
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
                {
                    rim::RIMIdType::Data,
                    RI_DATA_ERROR_LIST
                });
                
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

TEST_F(
    DataStoreWorkerFixture,
    StoreErrorListCopiesBinaryData)
{
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
                {
                    rim::RIMIdType::Data,
                    RI_DATA_ERROR_LIST
                });

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

TEST_F(
    DataStoreWorkerFixture,
    NoNotificationWhenUnchanged)
{
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

TEST_F(
    DataStoreWorkerFixture,
    SameBinaryDoesNotNotifyAgain)
{
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

TEST_F(
    DataStoreWorkerFixture,
    BinaryHashChangedNotifiesAgain)
{
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

TEST_F(
    DataStoreWorkerFixture,
    BinarySizeChangedNotifiesAgain)
{
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