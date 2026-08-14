#include <gtest/gtest.h>

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

TEST(
    RIMSnapshotTest,
    FindExistingItem)
{
    rim::RIMSnapshot snapshot;

    rim::RIMDataItem source{};

    source.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    source.valueType =
        rim::ValueType::kDouble;

    source.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    snapshot.items.push_back(
        source);

    rim::RIMDataItem result{};

    EXPECT_TRUE(
        snapshot.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            result));

    double value{};

    EXPECT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            result.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    RIMSnapshotTest,
    ReturnFalseWhenNotFound)
{
    rim::RIMSnapshot snapshot;

    rim::RIMDataItem result{};

    EXPECT_FALSE(
        snapshot.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            result));
}

TEST(
    RIMSnapshotTest,
    TryGetDouble)
{
    rim::RIMSnapshot snapshot;

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.valueType =
        rim::ValueType::kDouble;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    snapshot.items.push_back(
        item);

    double value{};

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    RIMSnapshotTest,
    TryGetBinary)
{
    rim::RIMSnapshot snapshot;

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

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    snapshot.items.push_back(
        item);

    const rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        snapshot.TryGetBinary(
            RI_DATA_ERROR_LIST,
            found));

    ASSERT_NE(
        found,
        nullptr);

    EXPECT_EQ(
        found->data.size(),
        3U);
}