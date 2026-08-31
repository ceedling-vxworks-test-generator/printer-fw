#include <gtest/gtest.h>

#include "DeviceEvent.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"
#include "Product.hpp"

#include "test/core/support/StorageTestHelper.hpp"

TEST(
    EndToEndAdapterToStoreTest,
    StoreTemperature)
{
    rim::DeviceEvent event{};

    event.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    event.value =
        30;

    rim::PartitionStorageRegistry store;

    rim::DataDomainMap map(
        rim::kProductDefinition);

    rim::RIMDataItem item{};

    item.id =
        event.id;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            event.value);

    const auto domainId =
        map.Find(
            {
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    item.id)
            });
            
    ASSERT_NE(
        domainId,
        rim::kInvalidDomainId);

    auto& storage =
        rim::test::GetStorage(
            store,
            domainId);

    storage.Store(
        item);

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        storage.Find(
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