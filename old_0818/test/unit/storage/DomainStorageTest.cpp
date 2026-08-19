#include <gtest/gtest.h>

#include "printer_a.h"

#include "DomainStorage.hpp"
#include "RIMValueAccessor.hpp"

namespace rim
{

namespace
{

RIMDataItem CreateItem(
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;

    item.valueType =
        ValueType::kInt32;

    item.value.type =
        ValueType::kInt32;

    item.value.value.i32 =
        value;

    return item;
}

} // namespace

TEST(
    DomainStorageTest,
    StoreAndGet)
{
    DomainStorage storage;

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
    DomainStorageTest,
    OverwriteValue)
{
    DomainStorage storage;

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
    DomainStorageTest,
    UnknownData)
{
    DomainStorage storage;

    RIMDataItem out{};

    EXPECT_FALSE(
        storage.Find(
            RI_DATA_ERROR_LIST,
            out));
}

TEST(
    DomainStorageTest,
    GetAllReturnsStoredItems)
{
    DomainStorage storage;

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
    DomainStorageTest,
    VersionIncrement)
{
    DomainStorage storage;

    EXPECT_EQ(
        storage.GetVersion().value,
        0u);

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    EXPECT_EQ(
        storage.GetVersion().value,
        1u);

    storage.Store(
        CreateItem(
            RI_DATA_HUMIDITY_SENSOR,
            20));

    EXPECT_EQ(
        storage.GetVersion().value,
        2u);
}

TEST(
    DomainStorageTest,
    OverwriteAlsoIncrementsVersion)
{
    DomainStorage storage;

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    const auto before =
        storage.GetVersion().value;

    storage.Store(
        CreateItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            20));

    EXPECT_GT(
        storage.GetVersion().value,
        before);
}

} // namespace rim