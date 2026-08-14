#include <gtest/gtest.h>

#include "PrinterAProductDefinition.hpp"
#include "ProductDefinition.hpp"

TEST(ProductDefinitionLookupTest,
     FindExistingDataItem)
{
    EXPECT_NE(
        rim::FindDataItem(
            rim::kPrinterAProductDefinition,
            "TemperatureSensorA"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     FindExistingCapability)
{
    EXPECT_NE(
        rim::FindCapability(
            rim::kPrinterAProductDefinition,
            "Environment"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     FindExistingPipeline)
{
    EXPECT_NE(
        rim::FindPipeline(
            rim::kPrinterAProductDefinition,
            "EnvironmentPipeline"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     FindExistingDomain)
{
    EXPECT_NE(
        rim::FindDomain(
            rim::kPrinterAProductDefinition,
            "Environment"),
        nullptr);
}

TEST(ProductDefinitionLookupTest,
     EnumerateCapabilities)
{
    const auto* capabilities =
        rim::GetCapabilities(
            rim::kPrinterAProductDefinition);

    const auto count =
        rim::GetCapabilityCount(
            rim::kPrinterAProductDefinition);

    EXPECT_GT(count, 0u);
    EXPECT_NE(capabilities, nullptr);
}

TEST(ProductDefinitionLookupTest,
     EnumeratePipelines)
{
    const auto* pipelines =
        rim::GetPipelines(
            rim::kPrinterAProductDefinition);

    const auto count =
        rim::GetPipelineCount(
            rim::kPrinterAProductDefinition);

    EXPECT_GT(count, 0u);
    EXPECT_NE(pipelines, nullptr);
}

TEST(ProductDefinitionLookupTest,
     EnumerateDataItems)
{
    EXPECT_NE(
        rim::GetDataItems(
            rim::kPrinterAProductDefinition),
        nullptr);

    EXPECT_GT(
        rim::GetDataItemCount(
            rim::kPrinterAProductDefinition),
        0u);
}

TEST(ProductDefinitionLookupTest,
     EnumerateDomains)
{
    EXPECT_NE(
        rim::GetDomains(
            rim::kPrinterAProductDefinition),
        nullptr);

    EXPECT_GT(
        rim::GetDomainCount(
            rim::kPrinterAProductDefinition),
        0u);
}

TEST(
    ProductDefinitionLookupTest,
    FindCapabilityById)
{
    EXPECT_NE(
        rim::FindCapability(
            rim::kPrinterAProductDefinition,
            RI_CAPABILITY_ENVIRONMENT),
        nullptr);
}

TEST(
    ProductDefinitionLookupTest,
    FindErrorList)
{
    const auto* item =
        rim::FindDataItem(
            rim::kPrinterAProductDefinition,
            RI_DATA_ERROR_LIST);

    ASSERT_NE(
        item,
        nullptr);

    EXPECT_EQ(
        item->storeValueType,
        rim::ValueType::kBinary);
}