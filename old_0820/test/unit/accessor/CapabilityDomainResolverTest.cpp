#include <gtest/gtest.h>

#include <unordered_set>

#include "CapabilityDomainResolver.hpp"
#include "PrinterAProductDefinition.hpp"
#include "DomainId.hpp"

namespace rim
{

TEST(
    CapabilityDomainResolverTest,
    EnvironmentCapabilityReturnsDomains)
{
    CapabilityDomainResolver resolver(
        kPrinterAProductDefinition);

    const auto* capability =
        FindCapability(
            kPrinterAProductDefinition,
            kEnvironmentCapability.id);

    ASSERT_NE(
        capability,
        nullptr);

    const auto result =
        resolver.ResolveDomains(
            *capability);

    EXPECT_FALSE(
        result.empty());

    for (const auto domainId :
         result)
    {
        EXPECT_NE(
            domainId,
            kInvalidDomainId);
    }
}

TEST(
    CapabilityDomainResolverTest,
    DomainsAreUnique)
{
    CapabilityDomainResolver resolver(
        kPrinterAProductDefinition);

    const auto* capability =
        FindCapability(
            kPrinterAProductDefinition,
            kEnvironmentCapability.id);

    ASSERT_NE(
        capability,
        nullptr);

    const auto result =
        resolver.ResolveDomains(
            *capability);

    std::unordered_set<DomainId>
        uniqueDomains(
            result.begin(),
            result.end());

    EXPECT_EQ(
        uniqueDomains.size(),
        result.size());
}

} // namespace rim