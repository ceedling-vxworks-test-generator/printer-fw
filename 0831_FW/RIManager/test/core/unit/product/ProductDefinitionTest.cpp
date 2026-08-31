#include <gtest/gtest.h>

#include "Product.hpp"
#include "ProductDefinition.hpp"
#include "DomainDefinition.hpp"
#include "RIMValue.hpp"
#include "RIMValueFactory.hpp"
#include "Route/RouteId.hpp"

namespace
{

rim::RIMValue DummyBuild(
    const rim::RIMSnapshot&)
{
    return rim::RIMValueFactory::CreateBool(false);
}

bool DummyCompare(
    const rim::RIMValue&,
    const rim::RIMValue&)
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
     ProductDefinitionIsValid)
{
    EXPECT_TRUE(
        rim::ValidateProductDefinition(
            rim::kProductDefinition));
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

        // nullptr,
        // 0
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
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

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

            {
                rim::ValueType::kBool,
                { .b = false },
                0
            },

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        },
        {
            RI_CAPABILITY_PRINT_READY,
            "CapabilityA",
            rim::ValueType::kBool,

            {
                rim::ValueType::kBool,
                { .b = false },
                0
            },

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

        // nullptr,
        // 0
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
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

            nullptr,
            nullptr,
            nullptr
        },
        {
            RI_DATA_TEMPERATURE_SENSOR_B,
            "Duplicate",
            &kDomainA,
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

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

        // nullptr,
        // 0
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
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

            nullptr,
            nullptr,
            nullptr
        },
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemB",
            &kDomainA,
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

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

        // nullptr,
        // 0
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
            rim::ROUTE_VALUE ,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

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

            {
                rim::ValueType::kBool,
                { .b = false },
                0
            },

            DummyBuild,
            DummyCompare,

            requiredDataIds,
            1
        },
        {
            RI_CAPABILITY_ENVIRONMENT,
            "CapabilityB",
            rim::ValueType::kBool,

            {
                rim::ValueType::kBool,
                { .b = false },
                0
            },

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

        // nullptr,
        // 0
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
            1
        }
    };

    const rim::DataItemDefinition dataItems[]
    {
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            "ItemA",
            &kDomainA,
            rim::ROUTE_VALUE,

            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,
            rim::ValueType::kDouble,

            {
                rim::ValueType::kDouble,
                { .d = 0.0 },
                0
            },

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

        // nullptr,
        // 0
    };

    EXPECT_FALSE(
        rim::ValidateProductDefinition(
            product));
}

TEST(
    ProductDefinitionTest,
    DataItemHasPublisherSettings)
{
    const auto* item =
        rim::FindDataItem(
            rim::kProductDefinition,
            RI_DATA_TEMPERATURE_SENSOR_A);

    ASSERT_NE(
        item,
        nullptr);

    EXPECT_EQ(
        item->publisherCompressionPolicy,
        rim::CompressionPolicy::KeepLatest);

    EXPECT_EQ(
        item->MinimumNotifyIntervalMs,
        100U);
}

TEST(
    ProductDefinitionTest,
    CapabilityHasPublisherSettings)
{
    const auto* capability =
        rim::FindCapability(
            rim::kProductDefinition,
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        capability,
        nullptr);

    EXPECT_EQ(
        capability->publisherCompressionPolicy,
        rim::CompressionPolicy::KeepLatest);

    EXPECT_EQ(
        capability->MinimumNotifyIntervalMs,
        100U);
}

TEST(
    ProductDefinitionTest,
    FacadeHasPublisherSettings)
{
    const auto* facade =
        rim::FindFacade(
            rim::kProductDefinition,
            RI_FACADE_ENVIRONMENT_READY);

    ASSERT_NE(
        facade,
        nullptr);

    EXPECT_EQ(
        facade->publisherCompressionPolicy,
        rim::CompressionPolicy::KeepOldest);

    EXPECT_EQ(
        facade->MinimumNotifyIntervalMs,
        100U);
}