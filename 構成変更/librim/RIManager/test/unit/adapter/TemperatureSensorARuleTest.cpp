#include <gtest/gtest.h>

#include "TemperatureSensorAAdapter.hpp"

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

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
}