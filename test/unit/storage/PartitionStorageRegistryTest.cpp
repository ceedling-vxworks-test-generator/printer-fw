#include <gtest/gtest.h>

#include "printer_a.h"

#include "PartitionStorageRegistry.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMDataItem.hpp"
#include "ValueType.hpp"

namespace rim
{

namespace
{

RIMDataItem CreateInt32Item(
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;

    // item.valueType =
    //     ValueType::kInt32;

    item.value.type =
        ValueType::kInt32;

    item.value.value.i32 =
        value;

    return item;
}

} // namespace

TEST(
    PartitionStorageRegistryTest,
    GetOrCreateReturnsSameStorage)
{
    PartitionStorageRegistry registry;

    auto& a =
        registry.GetOrCreate(
            1);

    auto& b =
        registry.GetOrCreate(
            1);

    EXPECT_EQ(
        &a,
        &b);
}

TEST(
    PartitionStorageRegistryTest,
    DifferentDomainsReturnDifferentStorage)
{
    PartitionStorageRegistry registry;

    auto& a =
        registry.GetOrCreate(
            1);

    auto& b =
        registry.GetOrCreate(
            2);

    EXPECT_NE(
        &a,
        &b);
}

TEST(
    PartitionStorageRegistryTest,
    FindReturnsExistingStorage)
{
    PartitionStorageRegistry registry;

    auto& created =
        registry.GetOrCreate(
            1);

    const auto* found =
        registry.Find(
            1);

    ASSERT_NE(
        found,
        nullptr);

    EXPECT_EQ(
        found,
        &created);
}

TEST(
    PartitionStorageRegistryTest,
    FindUnknownReturnsNullptr)
{
    PartitionStorageRegistry registry;

    EXPECT_EQ(
        registry.Find(
            999),
        nullptr);
}

TEST(
    PartitionStorageRegistryTest,
    StoreItemsInDifferentDomains)
{
    PartitionStorageRegistry registry;

    auto& domain1 =
        registry.GetOrCreate(
            1);

    auto& domain2 =
        registry.GetOrCreate(
            2);

    const auto item1 =
        CreateInt32Item(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10);

    const auto item2 =
        CreateInt32Item(
            RI_DATA_HUMIDITY_SENSOR,
            20);

    domain1.Store(
        item1);

    domain2.Store(
        item2);

    RIMDataItem out1{};
    RIMDataItem out2{};

    EXPECT_TRUE(
        domain1.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out1));

    EXPECT_TRUE(
        domain2.Find(
            RI_DATA_HUMIDITY_SENSOR,
            out2));

    std::int32_t value1{};
    std::int32_t value2{};

    ASSERT_TRUE(
        RIMValueAccessor::GetInt32(
            out1.value,
            value1));

    ASSERT_TRUE(
        RIMValueAccessor::GetInt32(
            out2.value,
            value2));

    EXPECT_EQ(
        value1,
        10);

    EXPECT_EQ(
        value2,
        20);
}

TEST(
    PartitionStorageRegistryTest,
    DifferentDomainsRemainIndependent)
{
    PartitionStorageRegistry registry;

    auto& domain1 =
        registry.GetOrCreate(1);

    auto& domain2 =
        registry.GetOrCreate(2);

    domain1.Store(
        CreateInt32Item(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    RIMDataItem out{};

    EXPECT_FALSE(
        domain2.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));
}

} // namespace rim