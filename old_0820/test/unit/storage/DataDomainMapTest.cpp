#include <gtest/gtest.h>

#include "DataDomainMap.hpp"
#include "DomainId.hpp"

#include "PrinterAProductDefinition.hpp"
#include "DataItem/PrinterADataItems.hpp"

namespace rim
{

TEST(
    DataDomainMapTest,
    ExistingDataReturnsDomain)
{
    DataDomainMap map(
        kPrinterAProductDefinition);

    const DomainId domainId =
        map.Find(
            kTemperatureSensorA.id);

    EXPECT_NE(
        domainId,
        kInvalidDomainId);
}

TEST(
    DataDomainMapTest,
    UnknownDataReturnsInvalidDomain)
{
    DataDomainMap map(
        kPrinterAProductDefinition);

    EXPECT_EQ(
        map.Find(
            static_cast<RIDataId>(
                0xFFFFFFFF)),
        kInvalidDomainId);
}

TEST(
    DataDomainMapTest,
    SameDomainReturnsSameDomainId)
{
    DataDomainMap map(
        kPrinterAProductDefinition);

    const DomainId domainA =
        map.Find(
            kTemperatureSensorA.id);

    EXPECT_NE(
        domainA,
        kInvalidDomainId);

    EXPECT_EQ(
        map.Find(
            kTemperatureSensorA.id),
        domainA);
}

} // namespace rim