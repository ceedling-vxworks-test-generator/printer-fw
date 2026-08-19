#include <gtest/gtest.h>

#include "RouteResolver.hpp"
#include "PrinterAProductDefinition.hpp"

TEST(
    RouteResolverTest,
    ResolveCapabilityRoute)
{
    rim::NotificationTarget target
    {
        rim::NotificationTargetType::Capability,
        RI_CAPABILITY_ENVIRONMENT
    };

    const auto* route =
        rim::RouteResolver::Resolve(
            rim::kPrinterAProductDefinition,
            target);

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
    rim::NotificationTarget target
    {
        rim::NotificationTargetType::Data,
        RI_DATA_TEMPERATURE_SENSOR_A
    };

    const auto* route =
        rim::RouteResolver::Resolve(
            rim::kPrinterAProductDefinition,
            target);

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->name,
        "ValueRoute");
}