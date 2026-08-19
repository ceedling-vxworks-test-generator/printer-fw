#include <gtest/gtest.h>

#include <any>

#include "PrinterAProductDefinition.hpp"
#include "ProductDefinition.hpp"
#include "DomainDefinition.hpp"

namespace
{

std::any DummyBuild(
    const rim::RIMSnapshot&)
{
    return std::any{};
}

bool DummyCompare(
    const std::any&,
    const std::any&)
{
    return false;
}

inline constexpr rim::DomainDefinition
kDomainA
{
    "DomainA"
};

}

TEST(ProductDefinitionTest,
     PrinterAProductDefinitionIsValid)
{
    EXPECT_TRUE(
        rim::ValidateProductDefinition(
            rim::kPrinterAProductDefinition));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicateDomain)
{
    const rim::DomainDefinition domains[]
    {
        { "DomainA" },
        { "DomainA" }
    };

    const rim::ProductDefinition product
    {
        domains,
        2,

        nullptr,
        0,

        nullptr,
        0,

        nullptr,
        0,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicateCapability)
{
    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        }
    };

    const RIDataId requiredDataIds[]
    {
        RI_DATA_TEMPERATURE_SENSOR_A
    };

    const rim::CapabilityItemDefinition capabilities[]
    {
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityA",
            rim::ValueType::kBool,

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        },
        {
            RI_CAPABILITY_PRINT_READY,
            "CapabilityA",
            rim::ValueType::kBool,

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        dataItems,
        1,

        capabilities,
        2,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicateDataItemName)
{
    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "Duplicate",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        },
        {
            RI_DATA_TEMPERATURE_SENSOR_B,
            "Duplicate",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        dataItems,
        2,

        nullptr,
        0,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicateDataItemId)
{
    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        },
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemB",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        dataItems,
        2,

        nullptr,
        0,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicatePipeline)
{
    const std::string_view domains[]
    {
        "DomainA"
    };

    const rim::PipelineDefinition pipelines[]
    {
        {
            "PipelineA",
            domains,
            1,
            "CapabilityA"
        },
        {
            "PipelineA",
            domains,
            1,
            "CapabilityB"
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        nullptr,
        0,

        nullptr,
        0,

        pipelines,
        2
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDuplicateCapabilityId)
{
    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            &kDomainA,
            "ValueRoute",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        }
    };

    const RIDataId requiredDataIds[]
    {
        RI_DATA_TEMPERATURE_SENSOR_A
    };

    const rim::CapabilityItemDefinition capabilities[]
    {
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityA",
            rim::ValueType::kBool,

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        },
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityB",
            rim::ValueType::kBool,

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        dataItems,
        1,

        capabilities,
        2,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RejectDataItemWithUnknownRoute)
{
    const rim::RouteDefinition routes[]
    {
        {
            "ValueRoute"
        }
    };

    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            &kDomainA,
            "RouteA",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        routes,
        1,

        dataItems,
        1,

        nullptr,
        0,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    RouteHasCompressionPolicy)
{
    const auto* route =
        rim::FindRoute(
            rim::kPrinterAProductDefinition,
            "ValueRoute");

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->compressionPolicy,
        rim::CompressionPolicy::KeepLatest);
}

TEST(
    ProductDefinitionTest,
    RouteHasMinimumNotifyInterval)
{
    const auto* route =
        rim::FindRoute(
            rim::kPrinterAProductDefinition,
            "ValueRoute");

    ASSERT_NE(
        route,
        nullptr);

    EXPECT_EQ(
        route->minimumNotifyIntervalMs,
        100U);
}