#include <gtest/gtest.h>

#include "ProductContext.hpp"
#include "Product.hpp"
#include "rim_api.h"

TEST(
    ProductContextTest,
    FindDataItemById)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    EXPECT_NE(
        context.FindDataItem(
            {
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    RI_DATA_ERROR_LIST)
            }),
        nullptr);
}

TEST(
    ProductContextTest,
    FindCapabilityById)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    EXPECT_NE(
        context.FindCapability(
        {
            rim::RIMIdType::Capability,
            static_cast<std::uint32_t>(
                RI_CAPABILITY_ENVIRONMENT)
        }),
        nullptr);
}

TEST(
    ProductContextTest,
    UnknownDataReturnsNull)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    EXPECT_EQ(
        context.FindDataItem(
            {
                rim::RIMIdType::Data,
                0xFFFFFFFFU
            }),
        nullptr);
}

TEST(
    ProductContextTest,
    RepeatedLookupReturnsSamePointer)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    auto* first =
        context.FindDataItem(
            {
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    RI_DATA_ERROR_LIST)
            });

    auto* second =
        context.FindDataItem(
            {
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    RI_DATA_ERROR_LIST)
            });
            
    EXPECT_EQ(
        first,
        second);
}

TEST(
    ProductContextTest,
    DebugCapabilityDependencies)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto& capabilities =
        context.AffectedCapabilities()
            .Find(
                {
                    rim::RIMIdType::Data,
                    RI_DATA_TEMPERATURE_SENSOR_A
                });

    std::cout
        << "Capability Count = "
        << capabilities.size()
        << std::endl;

    for (const auto* capability : capabilities)
    {
        ASSERT_NE(
            capability,
            nullptr);

        std::cout
            << capability->name
            << std::endl;
    }
}

TEST(
    ProductContextTest,
    DebugFacadeDependencies)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto& facades =
        context.AffectedFacades()
            .Find(
                RI_CAPABILITY_ENVIRONMENT);

    std::cout
        << "Facade Count = "
        << facades.size()
        << std::endl;

    for (const auto* facade : facades)
    {
        ASSERT_NE(
            facade,
            nullptr);

        std::cout
            << facade->name
            << std::endl;
    }
}

TEST(
    ProductContextTest,
    DebugCapabilityDomains)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto& domains =
        context.CapabilityRequiredDomains();

    (void)domains;

    SUCCEED();
}

TEST(
    ProductContextTest,
    DebugFacadeCapabilities)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto& capabilities =
        context.FacadeRequiredCapabilities()
            .GetRequiredCapabilities(
                RI_FACADE_ENVIRONMENT_READY);

    std::cout
        << "Capability Count = "
        << capabilities.size()
        << std::endl;

    for (auto capabilityId : capabilities)
    {
        std::cout
            << static_cast<uint32_t>(
                capabilityId)
            << std::endl;
    }
}

TEST(
    ProductContextTest,
    DataItemHasPublisherSettings)
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

    EXPECT_GT(
        item->MinimumNotifyIntervalMs,
        0U);
}

TEST(
    ProductContextTest,
    CapabilityHasPublisherSettings)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto* capability =
        context.FindFacade(
        {
            rim::RIMIdType::Facade,
            static_cast<std::uint32_t>(
                RI_FACADE_ENVIRONMENT_READY)
        });

    ASSERT_NE(
        capability,
        nullptr);

    EXPECT_GT(
        capability->MinimumNotifyIntervalMs,
        0U);
}

TEST(
    ProductContextTest,
    FacadeHasPublisherSettings)
{
    rim::ProductContext context(
        rim::kProductDefinition);

    const auto* facade =
        context.FindFacade(
            {
                rim::RIMIdType::Facade,
                static_cast<std::uint32_t>(
                    RI_FACADE_ENVIRONMENT_READY)
            });

    ASSERT_NE(
        facade,
        nullptr);

    EXPECT_GT(
        facade->MinimumNotifyIntervalMs,
        0U);
}

