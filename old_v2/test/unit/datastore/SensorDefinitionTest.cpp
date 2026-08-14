#include <gtest/gtest.h>

#include "products/printer_a/DataItem/PrinterADataDefinitionProvider.hpp"

TEST(
    PrinterADataDefinitionProviderTest,
    SensorDefinitions)
{
    rim::PrinterADataDefinitionProvider provider;

    const auto& sensors =
        provider.GetSensorDefinitions();

    EXPECT_FALSE(
        sensors.empty());

    EXPECT_EQ(
        sensors.size(),
        6U);
}