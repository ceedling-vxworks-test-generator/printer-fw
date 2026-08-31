#include <gtest/gtest.h>

#include "RIMId.hpp"
#include "CapabilitySnapshotProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"

#include "Product.hpp"
#include "ProductContext.hpp"

#include "test/core/support/StorageTestHelper.hpp"

TEST(
    CapabilitySnapshotProviderTest,
    CreateEnvironmentSnapshot)
{
    rim::PartitionStorageRegistry store;

    auto& domain =
        rim::test::GetStorage(
            store,
            1U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    domain.Store(
        item);

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor accessor(
        store,
        productContext);

    rim::CapabilitySnapshotProvider provider(
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
        rim::test::GetStorage(
            store,
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

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor accessor(
        store,
        productContext);

    rim::CapabilitySnapshotProvider provider(
        snapshotResolver,
        accessor);

    const auto snapshot =
        provider.Create(
            RI_CAPABILITY_ENVIRONMENT);

    rim::RIMDataItem temperature{};
    rim::RIMDataItem humidity{};

    EXPECT_TRUE(
        snapshot.Find(
            RI_DATA_TEMPERATURE_SENSOR_A,
            temperature));

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
        300.15,
        temperatureValue);

    EXPECT_DOUBLE_EQ(
        60.0,
        humidityValue);
}

TEST(
    CapabilitySnapshotProviderTest,
    UnknownCapabilityReturnsEmptySnapshot)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor accessor(
        store,
        productContext);

    rim::CapabilitySnapshotProvider provider(
        snapshotResolver,
        accessor);

    const auto snapshot =
        provider.Create(
            rim::kInvalidRIMObjectId);

    EXPECT_TRUE(
        snapshot.items.empty());
}

TEST(
    CapabilitySnapshotProviderTest,
    CollectItemsFromMultipleDomains)
{
    rim::PartitionStorageRegistry store;

    {
        auto& domain =
            rim::test::GetStorage(
                store,
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
            rim::test::GetStorage(
                store,
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

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            productContext);

    rim::SnapshotAccessor accessor(
        store,
        productContext);

    rim::CapabilitySnapshotProvider provider(
        snapshotResolver,
        accessor);

    const auto snapshot =
        provider.Create(
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_GE(
        snapshot.items.size(),
        1U);
}