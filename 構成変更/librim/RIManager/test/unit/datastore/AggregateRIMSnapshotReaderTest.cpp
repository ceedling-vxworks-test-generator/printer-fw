#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "AggregateRIMSnapshotReader.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

TEST(
    AggregateRIMSnapshotReaderTest,
    BuildSnapshot)
{
    rim::ValueStore store;

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperatureSensorA;

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
            rim::RIMDataId::kHumiditySensor;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        store.Store(item);
    }

    rim::RIMDataItem item{};

    {
        item.id =
            rim::RIMDataId::kTonerLevel;

        item.valueType =
            rim::ValueType::kInt32;

        item.value =
            rim::RIMValueFactory::CreateInt32(
                80);

        store.Store(item);
    }

    rim::AggregateRIMSnapshotReader reader(
        store);

    auto snapshot =
        reader.Read();

    // Legacy path

    EXPECT_DOUBLE_EQ(
        rim::GetDouble(
            snapshot,
            rim::RIMDataId::kTemperatureSensorA),
        300.15);

    EXPECT_DOUBLE_EQ(
        rim::GetDouble(
            snapshot,
            rim::RIMDataId::kHumiditySensor),
        60.0);

    // New generic path


    ASSERT_TRUE(
        snapshot.Find(
            rim::RIMDataId::kTemperatureSensorA,
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
            rim::RIMDataId::kHumiditySensor,
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
        snapshot.items.Size(),
        3U);


}