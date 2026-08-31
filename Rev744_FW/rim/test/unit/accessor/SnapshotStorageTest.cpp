#include <gtest/gtest.h>

#include "SnapshotStorage.hpp"

TEST(
    SnapshotStorageTest,
    StoreAndGet)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    const auto id =
        storage.Store(
            snapshot);

    rim::RIMSnapshot out;

    EXPECT_TRUE(
        storage.TryGet(
            id,
            out));
}

TEST(
    SnapshotStorageTest,
    ReturnFalseWhenNotFound)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    EXPECT_FALSE(
        storage.TryGet(
            999,
            snapshot));
}

TEST(
    SnapshotStorageTest,
    CountIncreases)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    EXPECT_EQ(
        storage.GetCount(),
        0u);

    storage.Store(
        snapshot);

    EXPECT_EQ(
        storage.GetCount(),
        1u);
}

TEST(
    SnapshotStorageTest,
    KeepMaximumCount)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    for (std::size_t i = 0;
         i < 100;
         ++i)
    {
        storage.Store(
            snapshot);
    }

    EXPECT_EQ(
        storage.GetCount(),
        64u);
}

TEST(
    SnapshotStorageTest,
    OldSnapshotEvicted)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    for (std::size_t i = 0;
         i < 65;
         ++i)
    {
        storage.Store(
            snapshot);
    }

    rim::RIMSnapshot out;

    EXPECT_FALSE(
        storage.TryGet(
            1,
            out));
}