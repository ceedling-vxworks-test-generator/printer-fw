#include <gtest/gtest.h>

#include "CapabilityDependencyMap.hpp"

#include "Product.hpp"

namespace rim
{

TEST(
    CapabilityDependencyMapTest,
    ExistingDataReturnsCapability)
{
    CapabilityDependencyMap map(
        kProductDefinition);

    const auto result =
        map.Find(
            {
                RIMIdType::Data,
                static_cast<std::uint32_t>(
                    kTemperatureSensorA.id)
            });

    EXPECT_FALSE(
        result.empty());
}

TEST(
    CapabilityDependencyMapTest,
    UnknownDataReturnsEmpty)
{
    CapabilityDependencyMap map(
        kProductDefinition);

    const auto result =
        map.Find(
            {
                RIMIdType::Data,
                0xFFFFFFFFu
            });

    EXPECT_TRUE(
        result.empty());
}

} // namespace rim