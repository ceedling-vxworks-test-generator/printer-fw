#include <gtest/gtest.h>

#include "DataStoreWorker.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "BinaryStoreValue.hpp"
#include "RIMValueFactory.hpp"

#include "CapabilityInputQueue.hpp"

#include "PrinterAProductDefinition.hpp"

TEST(
    DataStoreWorkerTest,
    ExecuteOnce)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        store,
        reader,
        capabilityQueue);

    rim::RIMDataItem item{};

    queue.Push(
        item);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    DataStoreWorkerTest,
    GenerateSnapshotEvent)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        store,
        reader,
        capabilityQueue);

    rim::RIMDataItem item{};

    queue.Push(
        item);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::CapabilityInput input{};

    EXPECT_TRUE(
        capabilityQueue.TryPop(
            input));
}

TEST(
    DataStoreWorkerTest,
    StoreErrorList)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        store,
        reader,
        capabilityQueue);

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
        rim::RIMDataId::kErrorList;

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    queue.Push(
        item);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::RIMDataItem storedItem{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kErrorList,
            storedItem));

    EXPECT_EQ(
        storedItem.value.type,
        rim::ValueType::kBinary);
}

TEST(
    DataStoreWorkerTest,
    StoreErrorListCopiesBinaryData)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        store,
        reader,
        capabilityQueue);

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
        rim::RIMDataId::kErrorList;

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    queue.Push(
        item);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        store.FindBinary(
            rim::RIMDataId::kErrorList,
            found));

    ASSERT_NE(
        found,
        nullptr);

    ASSERT_EQ(
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
    StoreErrorListKeepsOriginalData)
{
    rim::StoreInputQueue queue;

    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader reader(
        store);

    rim::CapabilityInputQueue capabilityQueue;

    rim::DataStoreWorker worker(
        rim::kPrinterAProductDefinition,
        queue,
        store,
        reader,
        capabilityQueue);

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
        rim::RIMDataId::kErrorList;

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