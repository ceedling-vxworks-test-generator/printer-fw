#include <gtest/gtest.h>

#include "datastore/AggregateRIMSnapshotReader.hpp"

#include "datastore/RIMValueFactory.hpp"

TEST(
    AggregateRIMSnapshotReaderTest,
    BuildSnapshot)
{
    rim::ValueStore store;

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperature;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        store.Store(item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kHumidify;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        store.Store(item);
    }

    rim::AggregateRIMSnapshotReader reader(
        store);

    auto snapshot =
        reader.Read();

    EXPECT_DOUBLE_EQ(
        snapshot.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        snapshot.humidity,
        60.0);
}