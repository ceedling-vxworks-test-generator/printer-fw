#include <gtest/gtest.h>

#include "AccessorFacade.hpp"

#include "AggregateRIMSnapshotReader.hpp"
#include "CapabilityAccessor.hpp"
#include "DataAccessor.hpp"
#include "MachineCapabilityStore.hpp"
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

    rim::MachineCapabilityStore capabilityStore;

    rim::AggregateRIMSnapshotReader
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

    rim::MachineCapabilityStore capabilityStore;

    rim::AggregateRIMSnapshotReader
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

    rim::MachineCapabilityStore capabilityStore;

    rim::AggregateRIMSnapshotReader
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
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
            rim::RIMDataId::kTemperatureSensorA);

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

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
            static_cast<rim::RIMDataId>(
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
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    valueStore.Store(
        {
            rim::RIMDataId::kHumiditySensor,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                50.0)
        });

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
                rim::RIMDataId::kTemperatureSensorA,
                rim::RIMDataId::kHumiditySensor
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
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    valueStore.Store(
        {
            rim::RIMDataId::kTemperatureSensorB,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                26.0)
        });

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
                rim::RIMDataId::kTemperatureSensorA,
                rim::RIMDataId::kTemperatureSensorB
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

    rim::MachineCapabilityStore
        capabilityStore;

    rim::AggregateRIMSnapshotReader
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
                static_cast<rim::RIMDataId>(
                    99999)
            });

    EXPECT_EQ(
        accessId,
        0u);
}