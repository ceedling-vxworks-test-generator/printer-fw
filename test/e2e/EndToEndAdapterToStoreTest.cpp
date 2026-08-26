#include <gtest/gtest.h>

#include "DeviceEvent.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "PrinterAProductDefinition.hpp"

TEST(
    EndToEndAdapterToStoreTest,
    StoreTemperature)
{
    rim::DeviceEvent event{};

    event.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    event.value = 30;

    rim::PartitionStorageRegistry store;

    rim::DataDomainMap map(
        rim::kPrinterAProductDefinition);

    rim::RIMDataItem item{};

    item.id = event.id;

    // item.valueType =
    //     rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            event.value);

    const auto domainId =
        map.Find(
            item.id);

    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    store
        .GetOrCreate(
            domainId)
        .Store(
            item);

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        store
            .GetOrCreate(
                domainId)
            .Find(
                RI_DATA_TEMPERATURE_SENSOR_A,
                out));

    std::int32_t value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            out.value,
            value));

    EXPECT_EQ(
        value,
        30);
}