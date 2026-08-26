#include <gtest/gtest.h>

#include "printer_a.h"

#include "PartitionStorage.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "BinaryInfo.hpp"

namespace rim
{

RIMDataItem CreateBinaryItem(
    RIDataId id,
    const std::uint8_t* data,
    std::size_t size)
{
    RIMDataItem item{};

    item.id = id;

    item.value =
        RIMValueFactory::CreateBinary(
            data,
            size);

    return item;
}

namespace
{

RIMDataItem CreateItem(
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;

    // item.valueType =
    //     ValueType::kInt32;

    item.value.type =
        ValueType::kInt32;

    item.value.value.i32 =
        value;

    return item;
}

} // namespace

TEST(
    PartitionStorageTest,
    StoreAndGet)
{
    PartitionStorage storage;

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            123));

    RIMDataItem out{};

    ASSERT_TRUE(
        storage.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));

    std::int32_t value{};

    ASSERT_TRUE(
        RIMValueAccessor::GetInt32(
            out.value,
            value));

    EXPECT_EQ(
        value,
        123);
}

TEST(
    PartitionStorageTest,
    OverwriteValue)
{
    PartitionStorage storage;

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            100));

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            200));

    RIMDataItem out{};

    ASSERT_TRUE(
        storage.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));

    std::int32_t value{};

    ASSERT_TRUE(
        RIMValueAccessor::GetInt32(
            out.value,
            value));

    EXPECT_EQ(
        value,
        200);
}

TEST(
    PartitionStorageTest,
    UnknownData)
{
    PartitionStorage storage;

    RIMDataItem out{};

    EXPECT_FALSE(
        storage.Find(
            RI_DATA_ERROR_LIST,
            out));
}

TEST(
    PartitionStorageTest,
    GetAllReturnsStoredItems)
{
    PartitionStorage storage;

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    storage.Store(
        CreateItem(
            RI_DATA_HUMIDITY_SENSOR,
            20));

    EXPECT_EQ(
        storage.GetAll().size(),
        2u);
}

TEST(
    PartitionStorageTest,
    GetBinaryHash)
{
    PartitionStorage storage;

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33
    };

    storage.Store(
        CreateBinaryItem(
            RI_DATA_ERROR_LIST,
            data,
            sizeof(data)));

    std::uint64_t hash{};

    EXPECT_TRUE(
        storage.GetBinaryHash(
            RI_DATA_ERROR_LIST,
            hash));

    EXPECT_NE(
        hash,
        0ULL);
}

TEST(
    PartitionStorageTest,
    BinaryHashChangesWhenDataChanges)
{
    PartitionStorage storage;

    const std::uint8_t data1[]
    {
        0x11,
        0x22,
        0x33
    };

    storage.Store(
        CreateBinaryItem(
            RI_DATA_ERROR_LIST,
            data1,
            sizeof(data1)));

    std::uint64_t hash1{};

    ASSERT_TRUE(
        storage.GetBinaryHash(
            RI_DATA_ERROR_LIST,
            hash1));

    const std::uint8_t data2[]
    {
        0x11,
        0x22,
        0x44
    };

    storage.Store(
        CreateBinaryItem(
            RI_DATA_ERROR_LIST,
            data2,
            sizeof(data2)));

    std::uint64_t hash2{};

    ASSERT_TRUE(
        storage.GetBinaryHash(
            RI_DATA_ERROR_LIST,
            hash2));

    EXPECT_NE(
        hash1,
        hash2);
}

TEST(
    PartitionStorageTest,
    GetBinaryInfo)
{
    PartitionStorage storage;

    const std::uint8_t data[]
    {
        0x11,
        0x22,
        0x33,
        0x44
    };

    storage.Store(
        CreateBinaryItem(
            RI_DATA_ERROR_LIST,
            data,
            sizeof(data)));

    BinaryInfo info{};

    ASSERT_TRUE(
        storage.GetBinaryInfo(
            RI_DATA_ERROR_LIST,
            info));

    ASSERT_NE(
        info.bytes,
        nullptr);

    EXPECT_EQ(
        info.size,
        sizeof(data));

    EXPECT_NE(
        info.hash,
        0ULL);

    EXPECT_EQ(
        info.bytes[0],
        0x11);

    EXPECT_EQ(
        info.bytes[1],
        0x22);

    EXPECT_EQ(
        info.bytes[2],
        0x33);

    EXPECT_EQ(
        info.bytes[3],
        0x44);
}

TEST(
    PartitionStorageTest,
    GetBinaryInfoUnknownReturnsFalse)
{
    PartitionStorage storage;

    BinaryInfo info{};

    EXPECT_FALSE(
        storage.GetBinaryInfo(
            RI_DATA_ERROR_LIST,
            info));
}

} // namespace rim