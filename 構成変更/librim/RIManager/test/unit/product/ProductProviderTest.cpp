#include <gtest/gtest.h>

#include "products/printer_a/Pipeline/PrinterAProvider.hpp"
#include "products/printer_a/Pipeline/PrinterAFactory.hpp"

TEST(
    ProductProviderTest,
    HasErrorRegistry)
{
    rim::PrinterAProvider provider;

    const auto& registry =
        provider.GetErrorRegistry();

    (void)registry;
}

TEST(
    ProductProviderTest,
    HasDataDefinitionProvider)
{
    rim::PrinterAProvider provider;

    const auto& definitions =
        provider.GetDataDefinitionProvider();

    EXPECT_FALSE(
        definitions.GetDefinitions().empty());
}

TEST(ProductProviderTest, HasProductDefinition)
{
    auto provider = rim::CreatePrinterAProvider();

    EXPECT_NE(
        &provider->GetProductDefinition(),
        nullptr);
}

TEST(ProductProviderTest,
     ProductDefinitionContainsCapabilities)
{
    rim::PrinterAProvider provider;

    const auto& product =
        provider.GetProductDefinition();

    EXPECT_GT(
        rim::GetCapabilityCount(product),
        0u);
}

TEST(ProductProviderTest,
     ProductDefinitionContainsPipelines)
{
    rim::PrinterAProvider provider;

    const auto& product =
        provider.GetProductDefinition();

    EXPECT_GT(
        rim::GetPipelineCount(product),
        0u);
}

