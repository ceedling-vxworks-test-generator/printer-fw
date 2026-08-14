#include <gtest/gtest.h>

#include "DataStoreDispatcher.hpp"

#include "RIMSnapshotManager.hpp"
#include "BinaryStoreValue.hpp"
#include "BufferedQueue.hpp"
#include "CapabilityInputQueue.hpp"
#include "IProductProvider.hpp"
#include "ProductFactory.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"
#include "ValueStore.hpp"

TEST(
    DataStoreDispatcherTest,
    GenerateSnapshotEvent)
{
    rim::BufferedQueue queue;

    rim::ValueStore store;

    rim::RIMSnapshotManager Reader(store);

    rim::CapabilityInputQueue capabilityQueue;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::DataStoreDispatcher dispatcher(
        productProvider->GetProfile().definition,
        queue,
        store,
        productProvider->GetChangeChecker(),
        Reader,
        capabilityQueue,
        0U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        capabilityQueue.TryPop(
            input));

    EXPECT_EQ(
        input.changedDataId,
        RI_DATA_UPPER_DOOR_OPEN);
}

TEST(
    DataStoreDispatcherTest,
    UpdateValueStore)
{
    rim::BufferedQueue queue;

    rim::ValueStore store;

    rim::RIMSnapshotManager Reader(store);

    rim::CapabilityInputQueue capabilityQueue;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::DataStoreDispatcher dispatcher(
        productProvider->GetProfile().definition,
        queue,
        store,
        productProvider->GetChangeChecker(),
        Reader,
        capabilityQueue,
        0U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::RIMDataItem stored{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_UPPER_DOOR_OPEN,
            stored));

    EXPECT_EQ(
        stored.value.type,
        rim::ValueType::kBool);
}

TEST(
    DataStoreDispatcherTest,
    StoreErrorList)
{
    rim::BufferedQueue queue;

    rim::ValueStore store;

    rim::RIMSnapshotManager Reader(store);

    rim::CapabilityInputQueue capabilityQueue;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::DataStoreDispatcher dispatcher(
        productProvider->GetProfile().definition,
        queue,
        store,
        productProvider->GetChangeChecker(),
        Reader,
        capabilityQueue,
        0U);

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

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::RIMDataItem stored{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_ERROR_LIST,
            stored));

    EXPECT_EQ(
        stored.value.type,
        rim::ValueType::kBinary);
}

TEST(
    DataStoreDispatcherTest,
    StoreErrorListCopiesBinaryData)
{
    rim::BufferedQueue queue;

    rim::ValueStore store;

    rim::RIMSnapshotManager Reader(store);

    rim::CapabilityInputQueue capabilityQueue;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::DataStoreDispatcher dispatcher(
        productProvider->GetProfile().definition,
        queue,
        store,
        productProvider->GetChangeChecker(),
        Reader,
        capabilityQueue,
        0U);

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

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        store.FindBinary(
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
    DataStoreDispatcherTest,
    NoNotificationWhenUnchanged)
{
    rim::BufferedQueue queue;

    rim::ValueStore store;

    rim::RIMSnapshotManager Reader(store);
    rim::CapabilityInputQueue capabilityQueue;

    auto productProvider =
        rim::CreatePrinterAProvider();

    rim::DataStoreDispatcher dispatcher(
        productProvider->GetProfile().definition,
        queue,
        store,
        productProvider->GetChangeChecker(),
        Reader,
        capabilityQueue,
        0U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    rim::CapabilityInput input{};

    ASSERT_TRUE(
        capabilityQueue.TryPop(
            input));

    ASSERT_TRUE(
        queue.push(
            item));

    ASSERT_TRUE(
        dispatcher.ExecuteOnce());

    EXPECT_FALSE(
        capabilityQueue.TryPop(
            input));
}