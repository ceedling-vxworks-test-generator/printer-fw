#include <gtest/gtest.h>

#include "adapter/TemperatureSensorAAdapter.hpp"

#include "datastore/RIMValueAccessor.hpp"
#include "datastore/RIMValueFactory.hpp"

TEST(
    TemperatureSensorAAdapterTest,
    StoreKelvinToValueStore)
{
    rim::ValueStore store;

    rim::TemperatureSensorAAdapter adapter;

    adapter.Store(
        rim::RIMValueFactory::CreateDouble(
            27.0),
        store);

    rim::RIMDataItem item{};

    ASSERT_TRUE(
        store.Find(
            rim::RIMDataId::kTemperature,
            item));

    double temperature{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            item.value,
            temperature));

    EXPECT_DOUBLE_EQ(
        temperature,
        300.15);
}