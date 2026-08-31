#include <gtest/gtest.h>

#include "Product.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    ProductContextTest,
    DataUsesExpectedRoute)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto* item =
        context.FindDataItem(
            {
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    RI_DATA_TEMPERATURE_SENSOR_A)
            });
            
    ASSERT_NE(
        item,
        nullptr);

    EXPECT_EQ(
        item->routeId,
        rim::ROUTE_VALUE);

    const auto* route =
        context.FindRoute(
            item->routeId);

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->name,
        "ValueRoute");
}