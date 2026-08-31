#include <gtest/gtest.h>

#include <unordered_set>

#include "CapabilityRequiredDomainMap.hpp"
#include "DomainId.hpp"
#include "Product.hpp"

namespace rim
{

TEST(
    CapabilityRequiredDomainMapTest,
    ExistingCapabilityReturnsDomains)
{
    CapabilityRequiredDomainMap map(
        kProductDefinition);

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
    CapabilityRequiredDomainMapTest,
    UnknownCapabilityReturnsEmpty)
{
    CapabilityRequiredDomainMap map(
        kProductDefinition);

    const auto& domains =
        map.Find(
            static_cast<RICapabilityId>(
                0xFFFFFFFF));

    EXPECT_TRUE(
        domains.empty());
}

TEST(
    CapabilityRequiredDomainMapTest,
    DomainsAreUnique)
{
    CapabilityRequiredDomainMap map(
        kProductDefinition);

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