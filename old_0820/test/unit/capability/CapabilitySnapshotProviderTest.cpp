#include <gtest/gtest.h>

#include "CapabilitySnapshotProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"

#include "DomainStorageRegistry.hpp"
#include "RIMValueFactory.hpp"

#include "PrinterAProductDefinition.hpp"

TEST(
    CapabilitySnapshotProviderTest,
    CreateEnvironmentSnapshot)
{
    rim::DomainStorageRegistry store;

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

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            store,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_EQ(
        snapshot.items.size(),
        1U);

    double value{};

    EXPECT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        300.15);
}

TEST(
    CapabilitySnapshotProviderTest,
    ReturnsAllItemsInCapabilityDomain)
{
    rim::DomainStorageRegistry store;

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

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            store,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_DATA_TEMPERATURE_SENSOR_A);

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
    UnknownDataReturnsEmptySnapshot)
{
    rim::DomainStorageRegistry store;

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            store,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
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
    rim::DomainStorageRegistry
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

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            store,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    const auto snapshot =
        provider.Create(
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_GE(
        snapshot.items.size(),
        1U);
}

