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
        rim::SnapshotStorage::MaxSnapshots());
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

TEST(
    SnapshotStorageTest,
    CountNeverExceedsMaximum)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    for (int i = 0; i < 100; ++i)
    {
        storage.Store(
            snapshot);
    }

    EXPECT_EQ(
        storage.GetCount(),
        rim::SnapshotStorage::MaxSnapshots());
}

TEST(
    SnapshotStorageTest,
    StoreReturnsIncreasingId)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    const auto id1 =
        storage.Store(
            snapshot);

    const auto id2 =
        storage.Store(
            snapshot);

    const auto id3 =
        storage.Store(
            snapshot);

    EXPECT_LT(
        id1,
        id2);

    EXPECT_LT(
        id2,
        id3);
}

TEST(
    SnapshotStorageTest,
    AllSnapshotsCanBeRetrievedBeforeCapacity)
{
    rim::SnapshotStorage storage;

    std::vector<rim::AccessId> ids;

    rim::RIMSnapshot snapshot;

    for (std::size_t i = 0; i < 16; ++i)
    {
        ids.push_back(
            storage.Store(
                snapshot));
    }

    for (const auto id : ids)
    {
        rim::RIMSnapshot result;

        EXPECT_TRUE(
            storage.TryGet(
                id,
                result));
    }
}

TEST(
    SnapshotStorageTest,
    OldestSnapshotEvictedAfterCapacityExceeded)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    const auto oldest =
        storage.Store(
            snapshot);

    for (std::size_t i = 0; i < 16; ++i)
    {
        storage.Store(
            snapshot);
    }

    rim::RIMSnapshot result;

    EXPECT_FALSE(
        storage.TryGet(
            oldest,
            result));
}

TEST(
    SnapshotStorageTest,
    NewestSnapshotIsRetained)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    rim::AccessId latest{};

    for (std::size_t i = 0; i < 32; ++i)
    {
        latest =
            storage.Store(
                snapshot);
    }

    rim::RIMSnapshot result;

    EXPECT_TRUE(
        storage.TryGet(
            latest,
            result));
}

TEST(
    SnapshotStorageTest,
    WrapAroundMultipleTimes)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    rim::AccessId latest{};

    for (std::size_t i = 0; i < 1000; ++i)
    {
        latest =
            storage.Store(
                snapshot);
    }

    EXPECT_EQ(
        storage.GetCount(),
        rim::SnapshotStorage::MaxSnapshots());

    rim::RIMSnapshot result;

    EXPECT_TRUE(
        storage.TryGet(
            latest,
            result));
}

TEST(
    SnapshotStorageTest,
    UnknownIdReturnsFalse)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    EXPECT_FALSE(
        storage.TryGet(
            999999,
            snapshot));
}

TEST(
    SnapshotStorageTest,
    AccessIdReturnsCorrectSnapshot)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshotA;
    rim::RIMSnapshot snapshotB;

    const auto idA =
        storage.Store(
            snapshotA);

    const auto idB =
        storage.Store(
            snapshotB);

    EXPECT_NE(
        idA,
        idB);

    rim::RIMSnapshot result;

    EXPECT_TRUE(
        storage.TryGet(
            idA,
            result));

    EXPECT_TRUE(
        storage.TryGet(
            idB,
            result));
}

TEST(
    SnapshotStorageTest,
    AccessIdMonotonicIncrease)
{
    rim::SnapshotStorage storage;

    const auto id1 =
        storage.Store(
            rim::RIMSnapshot{});

    const auto id2 =
        storage.Store(
            rim::RIMSnapshot{});

    const auto id3 =
        storage.Store(
            rim::RIMSnapshot{});

    EXPECT_LT(
        id1,
        id2);

    EXPECT_LT(
        id2,
        id3);
}

TEST(
    SnapshotStorageTest,
    AccessIdInvalidAfterEviction)
{
    rim::SnapshotStorage storage;

    std::vector<std::uint64_t> ids;

    for (std::size_t i = 0;
         i < rim::SnapshotStorage::MaxSnapshots() + 10;
         ++i)
    {
        ids.push_back(
            storage.Store(
                rim::RIMSnapshot{}));
    }

    rim::RIMSnapshot snapshot;

    EXPECT_FALSE(
        storage.TryGet(
            ids.front(),
            snapshot));

    EXPECT_TRUE(
        storage.TryGet(
            ids.back(),
            snapshot));
}

