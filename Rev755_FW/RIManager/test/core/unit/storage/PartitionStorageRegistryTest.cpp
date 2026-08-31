#include <gtest/gtest.h>

#include "rim_api.h"

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

    item.id =
        id;

    item.value.type =
        ValueType::kInt32;

    item.value.value.i32 =
        value;

    return item;
}

} // namespace

TEST(
    PartitionStorageRegistryTest,
    RegisterDomainSuccess)
{
    PartitionStorageRegistry registry;

    EXPECT_TRUE(
        registry.RegisterDomain(
            1));
}

TEST(
    PartitionStorageRegistryTest,
    RegisterDomainDuplicateFails)
{
    PartitionStorageRegistry registry;

    EXPECT_TRUE(
        registry.RegisterDomain(
            1));

    EXPECT_FALSE(
        registry.RegisterDomain(
            1));
}

TEST(
    PartitionStorageRegistryTest,
    FindReturnsExistingStorage)
{
    PartitionStorageRegistry registry;

    ASSERT_TRUE(
        registry.RegisterDomain(
            1));

    const auto* found =
        registry.Find(
            1);

    ASSERT_NE(
        found,
        nullptr);
}

TEST(
    PartitionStorageRegistryTest,
    FindMutableReturnsExistingStorage)
{
    PartitionStorageRegistry registry;

    ASSERT_TRUE(
        registry.RegisterDomain(
            1));

    auto* found =
        registry.FindMutable(
            1);

    ASSERT_NE(
        found,
        nullptr);
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
    FindMutableUnknownReturnsNullptr)
{
    PartitionStorageRegistry registry;

    EXPECT_EQ(
        registry.FindMutable(
            999),
        nullptr);
}

TEST(
    PartitionStorageRegistryTest,
    StoreItemsInDifferentDomains)
{
    PartitionStorageRegistry registry;

    ASSERT_TRUE(
        registry.RegisterDomain(
            1));

    ASSERT_TRUE(
        registry.RegisterDomain(
            2));

    auto* domain1 =
        registry.FindMutable(
            1);

    auto* domain2 =
        registry.FindMutable(
            2);

    ASSERT_NE(
        domain1,
        nullptr);

    ASSERT_NE(
        domain2,
        nullptr);

    domain1->Store(
        CreateInt32Item(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    domain2->Store(
        CreateInt32Item(
            RI_DATA_HUMIDITY_SENSOR,
            20));

    RIMDataItem out1{};
    RIMDataItem out2{};

    EXPECT_TRUE(
        domain1->Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out1));

    EXPECT_TRUE(
        domain2->Find(
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

    ASSERT_TRUE(
        registry.RegisterDomain(
            1));

    ASSERT_TRUE(
        registry.RegisterDomain(
            2));

    auto* domain1 =
        registry.FindMutable(
            1);

    auto* domain2 =
        registry.FindMutable(
            2);

    ASSERT_NE(
        domain1,
        nullptr);

    ASSERT_NE(
        domain2,
        nullptr);

    domain1->Store(
        CreateInt32Item(
            RI_DATA_TEMPERATURE_SENSOR_A,
            10));

    RIMDataItem out{};

    EXPECT_FALSE(
        domain2->Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            out));
}

} // namespace rim