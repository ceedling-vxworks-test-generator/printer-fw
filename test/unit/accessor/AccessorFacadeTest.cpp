#include <gtest/gtest.h>

#include "AccessorFacade.hpp"

#include "CapabilityAccessor.hpp"
#include "CapabilityStore.hpp"
#include "DataAccessor.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotAccessor.hpp"
#include "SnapshotStorage.hpp"

#include "DomainStorageRegistry.hpp"
#include "DomainStorage.hpp"
#include "DataDomainMap.hpp"

namespace
{

constexpr RIDataId kUnknownDataId =
    static_cast<RIDataId>(
        99999);

class AccessorFacadeTest :
    public ::testing::Test
{
protected:

    AccessorFacadeTest()
        :
        dataAccessor(
            domainStore,
            rim::kPrinterAProductDefinition),
        capabilityAccessor(
            capabilityStore),
        snapshotAccessor(
            domainStore,
            snapshotStorage,
            rim::kPrinterAProductDefinition),
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor),
        dataDomainMap(
            rim::kPrinterAProductDefinition)
    {
    }

    void StoreItem(
        const rim::RIMDataItem& item)
    {
        const auto domainId =
            dataDomainMap.Find(
                item.id);

        ASSERT_NE(
            domainId,
            rim::kInvalidDomainId);

        domainStore
            .GetOrCreate(
                domainId)
            .Store(
                item);
    }

    void
    StoreTemperatureA(
        double value = 25.0)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreTemperatureB(
        double value = 26.0)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_B,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreHumidity(
        double value = 50.0)
    {
        StoreItem(
            {
                RI_DATA_HUMIDITY_SENSOR,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    rim::RIMSnapshot
    GetSnapshot(
        const uint64_t accessId)
    {
        rim::RIMSnapshot snapshot;

        EXPECT_TRUE(
            facade.Snapshot().TryGetSnapshot(
                accessId,
                snapshot));

        return snapshot;
    }

    rim::DomainStorageRegistry
        domainStore;

    rim::CapabilityStore
        capabilityStore;

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataDomainMap
        dataDomainMap;

    rim::DataAccessor
        dataAccessor;

    rim::CapabilityAccessor
        capabilityAccessor;

    rim::SnapshotAccessor
        snapshotAccessor;

    rim::AccessorFacade
        facade;
};

} // namespace

TEST_F(
    AccessorFacadeTest,
    ExposeDataAccessor)
{
    EXPECT_NE(
        &facade.Data(),
        nullptr);
}

TEST_F(
    AccessorFacadeTest,
    ExposeCapabilityAccessor)
{
    EXPECT_NE(
        &facade.Capability(),
        nullptr);
}

TEST_F(
    AccessorFacadeTest,
    ExposeSnapshotAccessor)
{
    EXPECT_NE(
        &facade.Snapshot(),
        nullptr);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByUnknownDataId)
{
    const auto accessId =
        facade.CreateSnapshotByDataId(
            kUnknownDataId);

    EXPECT_EQ(
        accessId,
        0u);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDuplicateDomainDataIds)
{
    StoreTemperatureA();
    StoreTemperatureB();

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                RI_DATA_TEMPERATURE_SENSOR_B
            });

    const auto snapshot =
        GetSnapshot(
            accessId);

    EXPECT_EQ(
        snapshot.items.size(),
        2u);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDataIds)
{
    StoreTemperatureA();
    StoreHumidity();

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                RI_DATA_HUMIDITY_SENSOR
            });

    const auto snapshot =
        GetSnapshot(
            accessId);

    EXPECT_EQ(
        snapshot.items.size(),
        2u);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDataIdsReturnsZeroWhenEmpty)
{
    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {});

    EXPECT_EQ(
        accessId,
        0u);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDataIdContainsExpectedData)
{
    constexpr double kExpectedValue =
        25.0;

    StoreTemperatureA(
        kExpectedValue);

    const auto accessId =
        facade.CreateSnapshotByDataId(
            RI_DATA_TEMPERATURE_SENSOR_A);

    const auto snapshot =
        GetSnapshot(
            accessId);

    ASSERT_EQ(
        snapshot.items.size(),
        1u);

    EXPECT_EQ(
        snapshot.items[0].id,
        RI_DATA_TEMPERATURE_SENSOR_A);

    double value{};

    ASSERT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        kExpectedValue);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDataIdsIgnoresUnknownDataId)
{
    constexpr double kExpectedValue =
        25.0;

    StoreTemperatureA(
        kExpectedValue);

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                kUnknownDataId
            });

    const auto snapshot =
        GetSnapshot(
            accessId);

    ASSERT_EQ(
        snapshot.items.size(),
        1u);

    EXPECT_EQ(
        snapshot.items[0].id,
        RI_DATA_TEMPERATURE_SENSOR_A);

    double value{};

    ASSERT_TRUE(
        snapshot.TryGetDouble(
            RI_DATA_TEMPERATURE_SENSOR_A,
            value));

    EXPECT_DOUBLE_EQ(
        value,
        kExpectedValue);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotReturnsIncreasingAccessId)
{
    StoreTemperatureA();

    const auto accessId1 =
        facade.CreateSnapshotByDataId(
            RI_DATA_TEMPERATURE_SENSOR_A);

    const auto accessId2 =
        facade.CreateSnapshotByDataId(
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_GT(
        accessId2,
        accessId1);
}

TEST_F(
    AccessorFacadeTest,
    CreateSnapshotByDataIdsWithOnlyUnknownIdsReturnsZero)
{
    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                kUnknownDataId,
                static_cast<RIDataId>(
                    88888)
            });

    EXPECT_EQ(
        accessId,
        0u);
}