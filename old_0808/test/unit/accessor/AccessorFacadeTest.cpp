#include <gtest/gtest.h>

#include "AccessorFacade.hpp"

#include "RIMSnapshotManager.hpp"
#include "CapabilityAccessor.hpp"
#include "DataAccessor.hpp"
#include "CapabilityStore.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotAccessor.hpp"
#include "SnapshotStorage.hpp"
#include "ValueStore.hpp"

TEST(
    AccessorFacadeTest,
    ExposeDataAccessor)
{
    rim::ValueStore valueStore;

    rim::CapabilityStore capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    EXPECT_NE(
        &facade.Data(),
        nullptr);
}

TEST(
    AccessorFacadeTest,
    ExposeCapabilityAccessor)
{
    rim::ValueStore valueStore;

    rim::CapabilityStore capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    EXPECT_NE(
        &facade.Capability(),
        nullptr);
}

TEST(
    AccessorFacadeTest,
    ExposeSnapshotAccessor)
{
    rim::ValueStore valueStore;

    rim::CapabilityStore capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    EXPECT_NE(
        &facade.Snapshot(),
        nullptr);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByDomains)
{
    rim::ValueStore valueStore;

    valueStore.Store(
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDomains(
            {
                "Environment"
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        facade.Snapshot().TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        1u);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByDataId)
{
    rim::ValueStore valueStore;

    valueStore.Store(
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDataId(
            RI_DATA_TEMPERATURE_SENSOR_A);

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        facade.Snapshot().TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        1u);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByUnknownDataId)
{
    rim::ValueStore valueStore;

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDataId(
            static_cast<RIDataId>(
                99999));

    EXPECT_EQ(
        accessId,
        0u);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByDataIds)
{
    rim::ValueStore valueStore;

    valueStore.Store(
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    valueStore.Store(
        {
            RI_DATA_HUMIDITY_SENSOR,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                50.0)
        });

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                RI_DATA_HUMIDITY_SENSOR
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        facade.Snapshot().TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        2u);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByDuplicateDomainDataIds)
{
    rim::ValueStore valueStore;

    valueStore.Store(
        {
            RI_DATA_TEMPERATURE_SENSOR_A,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    valueStore.Store(
        {
            RI_DATA_TEMPERATURE_SENSOR_B,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                26.0)
        });

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                RI_DATA_TEMPERATURE_SENSOR_B
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        facade.Snapshot().TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        2u);
}

TEST(
    AccessorFacadeTest,
    CreateSnapshotByDataIdsReturnsZeroWhenNoValidDataId)
{
    rim::ValueStore valueStore;

    rim::CapabilityStore
        capabilityStore;

    rim::RIMSnapshotManager
        reader(
            valueStore);

    rim::SnapshotStorage
        snapshotStorage;

    rim::DataAccessor
        dataAccessor(
            valueStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilityAccessor
        capabilityAccessor(
            capabilityStore);

    rim::SnapshotAccessor
        snapshotAccessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::AccessorFacade
        facade(
            dataAccessor,
            capabilityAccessor,
            snapshotAccessor);

    const auto accessId =
        facade.CreateSnapshotByDataIds(
            {
                static_cast<RIDataId>(
                    99999)
            });

    EXPECT_EQ(
        accessId,
        0u);
}