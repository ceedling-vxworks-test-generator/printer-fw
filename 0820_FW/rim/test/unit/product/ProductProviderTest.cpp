#include <gtest/gtest.h>

#include "products/printer_a/Pipeline/PrinterAProvider.hpp"
#include "products/printer_a/Pipeline/PrinterAFactory.hpp"

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
    ProductDefinitionContainsPipelines)
{
    rim::PrinterAProvider provider;

    const auto& product =
        provider
            .GetProfile()
            .definition;

    EXPECT_GT(
        rim::GetPipelineCount(
            product),
        0u);
}

// TEST(
//     ProductProviderTest,
//     HasProfile)
// {
//     rim::PrinterAProvider provider;

//     const auto& profile =
//         provider.GetProfile();

//     EXPECT_NE(
//         profile.errorRegistry,
//         nullptr);

//     EXPECT_GT(
//         profile.definition.dataItemCount,
//         0u);
// }

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
