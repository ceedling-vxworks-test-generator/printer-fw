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
        rim::RIMDataId::kTemperatureSensorA;

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
            rim::RIMDataId::kTemperatureSensorA,
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
            rim::RIMDataId::kTemperatureSensorA,
            result));
}

TEST(
    RIMSnapshotTest,
    TryGetDouble)
{
    rim::RIMSnapshot snapshot;

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kTemperatureSensorA;

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
            rim::RIMDataId::kTemperatureSensorA,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}