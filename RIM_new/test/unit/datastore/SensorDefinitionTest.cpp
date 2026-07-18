#include <gtest/gtest.h>

#include "datastore/PrinterAProvider.hpp"

TEST(
    PrinterAProviderTest,
    SensorDefinitions)
{
    rim::PrinterAProvider provider;

    const auto& sensors =
        provider.GetSensorDefinitions();

    EXPECT_FALSE(
        sensors.empty());

    EXPECT_EQ(
        sensors.size(),
        6U);
}