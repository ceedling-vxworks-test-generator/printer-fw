#include <gtest/gtest.h>

#include "test/support/TestDataProcessor.hpp"

#include "RIMValueAccessor.hpp"
#include "BinaryStoreValue.hpp"

#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "PrinterAProductDefinition.hpp"

namespace
{

rim::RIMDataItem GetItem(
    rim::DomainStorageRegistry& store,
    RIDataId dataId)
{
    rim::DataDomainMap map(
        rim::kPrinterAProductDefinition);

    const auto domainId =
        map.Find(
            dataId);

    EXPECT_NE(
        domainId,
        rim::kInvalidDomainId);

    rim::RIMDataItem item{};

    EXPECT_TRUE(
        store
            .GetOrCreate(
                domainId)
            .Find(
                dataId,
                item));

    return item;
}

} // namespace

TEST(
    EndToEndDataToStoreTest,
    TemperatureSensorA)
{
    rim::DomainStorageRegistry store;

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
    rim::DomainStorageRegistry store;

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
    rim::DomainStorageRegistry store;

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
    rim::DomainStorageRegistry store;

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