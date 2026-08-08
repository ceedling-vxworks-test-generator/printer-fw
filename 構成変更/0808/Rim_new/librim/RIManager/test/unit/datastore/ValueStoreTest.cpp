#include <gtest/gtest.h>

#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"
#include "BinaryStoreValue.hpp"

#include "ValueStore.hpp"

TEST(
    ValueStoreTest,
    StoreAndFind)
{
    rim::ValueStore store;

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            100);

    store.Store(item);

    rim::RIMDataItem out{};

    ASSERT_TRUE(
        store.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
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

TEST(
    ValueStoreTest,
    StoreAndFindBinary)
{
    rim::ValueStore store;

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data =
    {
        0x10,
        0x20,
        0x30
    };

    rim::RIMDataItem item{};

    item.id =
        static_cast<RIDataId>(
            100);

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::CreateBinary(
            binary.release());

    store.Store(
        item);

    rim::BinaryStoreValue* found{};

    ASSERT_TRUE(
        store.FindBinary(
            static_cast<RIDataId>(
                100),
            found));

    ASSERT_NE(
        found,
        nullptr);

    ASSERT_EQ(
        found->data.size(),
        3U);

    EXPECT_EQ(
        found->data[0],
        0x10);

    EXPECT_EQ(
        found->data[1],
        0x20);

    EXPECT_EQ(
        found->data[2],
        0x30);
}
