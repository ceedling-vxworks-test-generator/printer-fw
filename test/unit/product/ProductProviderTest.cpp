#include <gtest/gtest.h>

#include "products/printer_a/Route/PrinterAProvider.hpp"
#include "products/printer_a/Route/PrinterAFactory.hpp"

TEST(
    ProductProviderTest,
    HasProductDefinition)
{
    auto provider =
        rim::CreatePrinterAProvider();

    EXPECT_NE(
        &provider
             ->GetProfile()
             .definition,
        nullptr);
}

TEST(
    ProductProviderTest,
    ProductDefinitionContainsCapabilities)
{
    rim::PrinterAProvider provider;

    const auto& product =
        provider
            .GetProfile()
            .definition;

    EXPECT_GT(
        rim::GetCapabilityCount(
            product),
        0u);
}

TEST(
    ProductProviderTest,
    ProfileContainsDefinition)
{
    rim::PrinterAProvider provider;

    const auto& profile =
        provider.GetProfile();

    EXPECT_GT(
        profile.definition.dataItemCount,
        0u);
}
