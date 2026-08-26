#include <gtest/gtest.h>

#include "PrinterAProductDefinition.hpp"
#include "test/support/NotificationTestHelper.hpp"

TEST(
    RouteResolverTest,
    ResolveCapabilityRoute)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    const auto* capability =
        context.FindCapability(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        capability,
        nullptr);

    const auto* route =
        context.FindRoute(
            capability->routeId);

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->name,
        "ValueRoute");
}

TEST(
    RouteResolverTest,
    ResolveDataRoute)
{
    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    const auto* dataItem =
        context.FindDataItem(
            RI_DATA_TEMPERATURE_SENSOR_A);

    ASSERT_NE(
        dataItem,
        nullptr);

    const auto* route =
        context.FindRoute(
            dataItem->routeId);

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->name,
        "ValueRoute");
}