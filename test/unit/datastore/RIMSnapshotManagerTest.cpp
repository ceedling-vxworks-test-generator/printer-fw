#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "RIMSnapshotManager.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

TEST(
    RIMSnapshotManagerTest,
    BuildSnapshot)
{
    rim::DomainStorageRegistry store;

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        constexpr rim::DomainId domainId = 1;

        store
            .GetOrCreate(domainId)
            .Store(item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        constexpr rim::DomainId domainId = 1;

        store
            .GetOrCreate(domainId)
            .Store(item);
    }

    rim::RIMDataItem item{};

    {
        item.id =
            RI_DATA_TONER_LEVEL;

        item.valueType =
            rim::ValueType::kInt32;

        item.value =
            rim::RIMValueFactory::CreateInt32(
                80);

        constexpr rim::DomainId domainId = 1;

        store
            .GetOrCreate(domainId)
            .Store(item);
    }

    rim::RIMSnapshotManager reader(
        store);

    auto snapshot =
        reader.Read();

    // Legacy path

    EXPECT_DOUBLE_EQ(
        rim::GetDouble(
            snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A),
        300.15);

    EXPECT_DOUBLE_EQ(
        rim::GetDouble(
            snapshot,
            RI_DATA_HUMIDITY_SENSOR),
        60.0);

    // New generic path


    ASSERT_TRUE(
        snapshot.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            item));

    double temperature{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            temperature));

    EXPECT_DOUBLE_EQ(
        temperature,
        300.15);

    ASSERT_TRUE(
        snapshot.Find(
            RI_DATA_HUMIDITY_SENSOR,
            item));

    double humidity{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            humidity));

    EXPECT_DOUBLE_EQ(
        humidity,
        60.0);




    EXPECT_EQ(
        snapshot.items.size(),
        3U);


}

TEST(
    RIMSnapshotManagerTest,
    ReadBinary)
{
    rim::DomainStorageRegistry store;

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data =
    {
        1,
        2,
        3
    };

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_ERROR_LIST;

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    constexpr rim::DomainId domainId = 1;

    store
        .GetOrCreate(domainId)
        .Store(item);

    rim::RIMSnapshotManager reader(
        store);

    auto snapshot =
        reader.Read();

    const rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        snapshot.TryGetBinary(
            RI_DATA_ERROR_LIST,
            found));

    ASSERT_NE(
        found,
        nullptr);

    ASSERT_EQ(
        found->data.size(),
        3U);

    EXPECT_EQ(
        found->data[0],
        1);
}