#include <gtest/gtest.h>

#include "PrinterAProductDefinition.hpp"
#include "ProductDefinition.hpp"

namespace
{

void DummyBuild(
    const rim::RIMSnapshot&,
    rim::MachineCapabilityStore&)
{
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
    constexpr rim::RIMDataId items[]
    {
        rim::RIMDataId::kTemperatureSensorA
    };

    const rim::CapabilityItemDefinition capabilities[]
    {
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityA",
            DummyBuild,
            items,
            1
        },
        {
            RI_CAPABILITY_PRINT_READY,
            "CapabilityA",
            DummyBuild,
            items,
            1
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

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
            rim::RIMDataId::kTemperatureSensorA,
            "Duplicate",
            "DomainA",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        },
        {
            rim::RIMDataId::kTemperatureSensorB,
            "Duplicate",
            "DomainA",

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
            rim::RIMDataId::kTemperatureSensorA,
            "ItemA",
            "DomainA",

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            nullptr,
            nullptr,
            nullptr
        },
        {
            rim::RIMDataId::kTemperatureSensorA,
            "ItemB",
            "DomainA",

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
    constexpr rim::RIMDataId items[]
    {
        rim::RIMDataId::kTemperatureSensorA
    };

    const rim::CapabilityItemDefinition capabilities[]
    {
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityA",
            DummyBuild,
            items,
            1
        },
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityB",
            DummyBuild,
            items,
            1
        }
    };

    const rim::ProductDefinition product
    {
        nullptr,
        0,

        nullptr,
        0,

        capabilities,
        2,

        nullptr,
        0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}