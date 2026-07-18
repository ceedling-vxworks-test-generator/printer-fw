#include <gtest/gtest.h>

#include "datastore/RIMValueAccessor.hpp"
#include "datastore/RIMValueFactory.hpp"

#include "datastore/ValueStore.hpp"

TEST(
    ValueStoreTest,
    StoreAndFind)
{
    rim::ValueStore store;

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kTemperature;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            100);

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
        100);
}