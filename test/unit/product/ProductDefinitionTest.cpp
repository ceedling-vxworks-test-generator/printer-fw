#include <gtest/gtest.h>

#include <any>

#include "PrinterAProductDefinition.hpp"
#include "ProductDefinition.hpp"

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
            "DomainA",
            "Value",

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
            "DomainA",
            "Value",

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
            "DomainA",
            "Value",

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
            "DomainA",
            "Value",

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
            "DomainA",
            "Value",

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
            "DomainA",
            "Value",

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
            "Value"
        }
    };

    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            "DomainA",
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