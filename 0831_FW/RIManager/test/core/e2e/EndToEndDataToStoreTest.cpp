#include <gtest/gtest.h>

#include "test/core/support/TestDataProcessor.hpp"

#include "RIMValueAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "Product.hpp"

namespace
{

rim::RIMDataItem GetItem(
    rim::PartitionStorageRegistry& store,
    RIDataId dataId)
{
    rim::DataDomainMap map(
        rim::kProductDefinition);

    const rim::RIMId id
    {
        rim::RIMIdType::Data,
        static_cast<std::uint32_t>(
            dataId)
    };

    const auto domainId =
        map.Find(
            id);

    EXPECT_NE(
        domainId,
        rim::kInvalidDomainId);

    rim::RIMDataItem item{};

    const auto* storage =
        store.Find(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    EXPECT_TRUE(
        storage->Find(
            dataId,
            item));

    return item;
}

} // namespace

TEST(
    EndToEndDataToStoreTest,
    TemperatureSensorA)
{
    rim::PartitionStorageRegistry store;

    rim::TestDataProcessor::
        ProcessTemperatureA(
            27.0,
            store);

    auto item =
        GetItem(
            store,
            RI_DATA_TEMPERATURE_SENSOR_A);

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    EndToEndDataToStoreTest,
    TemperatureSensorB)
{
    rim::PartitionStorageRegistry store;

    rim::TestDataProcessor::
        ProcessTemperatureB(
            300.15,
            store);

    auto item =
        GetItem(
            store,
            RI_DATA_TEMPERATURE_SENSOR_B);

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    EndToEndDataToStoreTest,
    HumiditySensor)
{
    rim::PartitionStorageRegistry store;

    rim::TestDataProcessor::
        ProcessHumidity(
            60.0,
            store);

    auto item =
        GetItem(
            store,
            RI_DATA_HUMIDITY_SENSOR);

    double value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        60.0);
}

TEST(
    EndToEndDataToStoreTest,
    UpperDoorOpen)
{
    rim::PartitionStorageRegistry store;

    rim::TestDataProcessor::
        StoreData(
            RI_DATA_UPPER_DOOR_OPEN,
            rim::RIMValueFactory::CreateBool(
                true),
            store);

    auto item =
        GetItem(
            store,
            RI_DATA_UPPER_DOOR_OPEN);

    bool opened{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            item.value,
            opened));

    EXPECT_TRUE(
        opened);
}