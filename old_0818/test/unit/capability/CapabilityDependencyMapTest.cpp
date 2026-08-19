#include <gtest/gtest.h>

#include "CapabilityDependencyMap.hpp"

#include "PrinterAProductDefinition.hpp"

namespace rim
{

TEST(
    CapabilityDependencyMapTest,
    ExistingDataReturnsCapability)
{
    CapabilityDependencyMap map(
        kPrinterAProductDefinition);

    const auto result =
        map.Find(
            kTemperatureSensorA.id);

    EXPECT_FALSE(
        result.empty());
}

TEST(
    CapabilityDependencyMapTest,
    UnknownDataReturnsEmpty)
{
    CapabilityDependencyMap map(
        kPrinterAProductDefinition);

    const auto result =
        map.Find(
            static_cast<RIDataId>(
                0xFFFFFFFF));

    EXPECT_TRUE(
        result.empty());
}

}