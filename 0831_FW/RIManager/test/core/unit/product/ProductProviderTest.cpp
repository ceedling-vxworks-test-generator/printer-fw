#include <gtest/gtest.h>

#include "Route/Provider.hpp"
#include "Route/Factory.hpp"

TEST(
    ProductProviderTest,
    HasProductDefinition)
{
    auto provider =
        rim::CreateProvider();

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
    rim::Provider provider;

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
    rim::Provider provider;

    const auto& profile =
        provider.GetProfile();

    EXPECT_GT(
        profile.definition.dataItemCount,
        0u);
}
