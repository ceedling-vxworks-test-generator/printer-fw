#include <gtest/gtest.h>

#include "DataDomainMap.hpp"
#include "DomainId.hpp"

#include "Product.hpp"
#include "DataItem/DataItems.hpp"

namespace rim
{

TEST(
    DataDomainMapTest,
    ExistingDataReturnsDomain)
{
    DataDomainMap map(
        kProductDefinition);

    const DomainId domainId =
        map.Find(
            {
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    kTemperatureSensorA.id)
            });
            
    EXPECT_NE(
        domainId,
        kInvalidDomainId);
}

TEST(
    DataDomainMapTest,
    UnknownDataReturnsInvalidDomain)
{
    DataDomainMap map(
        kProductDefinition);

    EXPECT_EQ(
        map.Find(
            {
                RIMIdType::Data,
                0xFFFFFFFFu
            }),
        kInvalidDomainId);
}

TEST(
    DataDomainMapTest,
    SameDomainReturnsSameDomainId)
{
    DataDomainMap map(
        kProductDefinition);

    const DomainId domainA =
        map.Find(
            {
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    kTemperatureSensorA.id)
            });
            
    EXPECT_NE(
        domainA,
        kInvalidDomainId);

    EXPECT_EQ(
        map.Find(
            {
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    kTemperatureSensorA.id)
            }),
        domainA);
}

} // namespace rim