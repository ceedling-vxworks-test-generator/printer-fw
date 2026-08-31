#include <gtest/gtest.h>

#include "ProductDefinition.hpp"
#include "Product.hpp"

TEST(ProductDefinitionLookupTest,
     FindExistingDataItem)
{
    EXPECT_NE(
        rim::FindDataItem(
            rim::kProductDefinition,
            "TemperatureSensorA"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     FindExistingCapability)
{
    EXPECT_NE(
        rim::FindCapability(
            rim::kProductDefinition,
            "Environment"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     FindExistingDomain)
{
    EXPECT_NE(
        rim::FindDomain(
            rim::kProductDefinition,
            "Environment"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     EnumerateCapabilities)
{
    const auto* capabilities =
        rim::GetCapabilities(
            rim::kProductDefinition);

    const auto count =
        rim::GetCapabilityCount(
            rim::kProductDefinition);

    EXPECT_GT(count, 0u);
    EXPECT_NE(capabilities, nullptr);
}

TEST(ProductDefinitionLookupTest,
     EnumerateDataItems)
{
    EXPECT_NE(
        rim::GetDataItems(
            rim::kProductDefinition),
        nullptr);

    EXPECT_GT(
        rim::GetDataItemCount(
            rim::kProductDefinition),
        0u);
}

TEST(ProductDefinitionLookupTest,
     EnumerateDomains)
{
    EXPECT_NE(
        rim::GetDomains(
            rim::kProductDefinition),
        nullptr);

    EXPECT_GT(
        rim::GetDomainCount(
            rim::kProductDefinition),
        0u);
}

TEST(
    ProductDefinitionLookupTest,
    FindCapabilityById)
{
    EXPECT_NE(
        rim::FindCapability(
            rim::kProductDefinition,
            RI_CAPABILITY_ENVIRONMENT),
        nullptr);
}

TEST(
    ProductDefinitionLookupTest,
    FindErrorList)
{
    const auto* item =
        rim::FindDataItem(
            rim::kProductDefinition,
            RI_DATA_ERROR_LIST);

    ASSERT_NE(
        item,
        nullptr);

    EXPECT_EQ(
        item->storeValueType,
        rim::ValueType::kBinary);
}