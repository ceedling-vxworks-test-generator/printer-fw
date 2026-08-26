#include <gtest/gtest.h>

#include "CapabilitySnapshotProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "RIMValueFactory.hpp"

#include "PrinterAProductDefinition.hpp"
#include "ProductContext.hpp"

TEST(
    CapabilitySnapshotProviderTest,
    CreateEnvironmentSnapshot)
{
    rim::PartitionStorageRegistry store;

    auto& domain =
        store.GetOrCreate(
            1U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    domain.Store(
        item);

    rim::ProductContext
        productContext(
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        accessor(
            store,
            productContext);

    rim::CapabilitySnapshotProvider
        provider(
            snapshotResolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        1U,
        snapshot.items.size());

    double value{};

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        300.15,
        value);
}

TEST(
    CapabilitySnapshotProviderTest,
    ReturnsAllItemsInCapabilityDomain)
{
    rim::PartitionStorageRegistry store;

    auto& domain =
        store.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        domain.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        domain.Store(
            item);
    }

    rim::ProductContext
        productContext(
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        accessor(
            store,
            productContext);

    rim::CapabilitySnapshotProvider
        provider(
            snapshotResolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_CAPABILITY_ENVIRONMENT);

    rim::RIMDataItem temperature{};

    EXPECT_TRUE(
        snapshot.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            temperature));

    rim::RIMDataItem humidity{};

    EXPECT_TRUE(
        snapshot.Find(
            RI_DATA_HUMIDITY_SENSOR,
            humidity));

    double temperatureValue{};
    double humidityValue{};

    EXPECT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            temperature.value,
            temperatureValue));

    EXPECT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            humidity.value,
            humidityValue));

    EXPECT_DOUBLE_EQ(
        temperatureValue,
        300.15);

    EXPECT_DOUBLE_EQ(
        humidityValue,
        60.0);
}

TEST(
    CapabilitySnapshotProviderTest,
    UnknownCapabilityReturnsEmptySnapshot)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext
        productContext(
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        accessor(
            store,
            productContext);

    rim::CapabilitySnapshotProvider
        provider(
            snapshotResolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_INVALID_DATA_ID);

    EXPECT_TRUE(
        snapshot.items.empty());
}

TEST(
    CapabilitySnapshotProviderTest,
    CollectItemsFromMultipleDomains)
{
    rim::PartitionStorageRegistry
        store;

    {
        auto& domain =
            store.GetOrCreate(
                1U);

        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        domain.Store(
            item);
    }

    {
        auto& domain =
            store.GetOrCreate(
                2U);

        rim::RIMDataItem item{};

        item.id =
            RI_DATA_UPPER_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                true);

        domain.Store(
            item);
    }

    rim::ProductContext
        productContext(
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor
        accessor(
            store,
            productContext);

    rim::CapabilitySnapshotProvider
        provider(
            snapshotResolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_GE(
        snapshot.items.size(),
        1U);
}

