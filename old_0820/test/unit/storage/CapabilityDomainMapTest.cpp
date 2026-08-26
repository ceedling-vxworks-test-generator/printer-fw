#include <gtest/gtest.h>

#include <unordered_set>

#include "CapabilityDomainMap.hpp"
#include "DomainId.hpp"
#include "PrinterAProductDefinition.hpp"

namespace rim
{

TEST(
    CapabilityDomainMapTest,
    ExistingCapabilityReturnsDomains)
{
    CapabilityDomainMap map(
        kPrinterAProductDefinition);

    const auto& domains =
        map.Find(
            kEnvironmentCapability.id);

    EXPECT_FALSE(
        domains.empty());

    for (const auto domainId :
         domains)
    {
        EXPECT_NE(
            domainId,
            kInvalidDomainId);
    }
}

TEST(
    CapabilityDomainMapTest,
    UnknownCapabilityReturnsEmpty)
{
    CapabilityDomainMap map(
        kPrinterAProductDefinition);

    const auto& domains =
        map.Find(
            static_cast<RICapabilityId>(
                0xFFFFFFFF));

    EXPECT_TRUE(
        domains.empty());
}

TEST(
    CapabilityDomainMapTest,
    DomainsAreUnique)
{
    CapabilityDomainMap map(
        kPrinterAProductDefinition);

    const auto& domains =
        map.Find(
            kEnvironmentCapability.id);

    std::unordered_set<DomainId>
        uniqueDomains(
            domains.begin(),
            domains.end());

    EXPECT_EQ(
        uniqueDomains.size(),
        domains.size());
}

} // namespace rim