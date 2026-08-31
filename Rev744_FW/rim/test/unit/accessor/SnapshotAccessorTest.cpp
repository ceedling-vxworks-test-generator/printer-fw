#include <gtest/gtest.h>

#include "AggregateRIMSnapshotReader.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotAccessor.hpp"
#include "SnapshotStorage.hpp"
#include "ValueStore.hpp"

TEST(
    SnapshotAccessorTest,
    CreateSnapshot)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot();

    EXPECT_GT(
        accessId,
        0u);
}

TEST(
    SnapshotAccessorTest,
    GetCreatedSnapshot)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot();

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        accessor.TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        1u);
}

TEST(
    SnapshotAccessorTest,
    ReturnFalseWhenSnapshotDoesNotExist)
{
    rim::ValueStore store;

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot snapshot;

    EXPECT_FALSE(
        accessor.TryGetSnapshot(
            99999,
            snapshot));
}

TEST(
    SnapshotAccessorTest,
    CreateEnvironmentSnapshot)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot(
            {
                "Environment"
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        accessor.TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        1u);
}

TEST(
    SnapshotAccessorTest,
    UnknownDomainReturnsEmptySnapshot)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot(
            {
                "UnknownDomain"
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        accessor.TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_TRUE(
        snapshot.items.empty());
}

TEST(
    SnapshotAccessorTest,
    EmptyDomainListReturnsEmptySnapshot)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot(
            {});

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        accessor.TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_TRUE(
        snapshot.items.empty());
}

TEST(
    SnapshotAccessorTest,
    DuplicateDomainIgnored)
{
    rim::ValueStore store;

    store.Store(
        {
            rim::RIMDataId::kTemperatureSensorA,
            rim::ValueType::kDouble,
            rim::RIMValueFactory::CreateDouble(
                25.0)
        });

    rim::AggregateRIMSnapshotReader
        reader(
            store);

    rim::SnapshotStorage
        snapshotStorage;

    rim::SnapshotAccessor
        accessor(
            reader,
            snapshotStorage,
            rim::kPrinterAProductDefinition);

    const auto accessId =
        accessor.CreateSnapshot(
            {
                "Environment",
                "Environment"
            });

    rim::RIMSnapshot snapshot;

    EXPECT_TRUE(
        accessor.TryGetSnapshot(
            accessId,
            snapshot));

    EXPECT_EQ(
        snapshot.items.size(),
        1u);
}
