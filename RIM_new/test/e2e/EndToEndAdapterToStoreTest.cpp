#include <gtest/gtest.h>

#include "datastore/DeviceEvent.hpp"
#include "datastore/ValueStore.hpp"
#include "datastore/RIMValueFactory.hpp"
#include "datastore/RIMValueAccessor.hpp"
TEST(
    EndToEndAdapterToStoreTest,
    StoreTemperature)
{
    rim::DeviceEvent event{};

    event.id =
        rim::RIMDataId::kTemperature;

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
            rim::RIMDataId::kTemperature,
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