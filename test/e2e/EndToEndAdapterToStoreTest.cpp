#include <gtest/gtest.h>

#include "DeviceEvent.hpp"
#include "ValueStore.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"
TEST(
    EndToEndAdapterToStoreTest,
    StoreTemperature)
{
    rim::DeviceEvent event{};

    event.id =
        rim::RIMDataId::kTemperatureSensorA;

    event.value = 30;

    rim::ValueStore store;

    rim::RIMDataItem item{};

    item.id = event.id;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            event.value);

    store.Store(item);

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kTemperatureSensorA,
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