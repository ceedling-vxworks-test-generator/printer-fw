#include <gtest/gtest.h>

#include "printer_a.h"
#include "CapabilityStorage.hpp"

namespace
{

TEST(
    CapabilityStorageTest,
    StoreAndFindBool)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_TRUE(
        std::any_cast<bool>(
            *value));
}

TEST(
    CapabilityStorageTest,
    StoreAndFindInt32)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        int32_t{123});

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_EQ(
        std::any_cast<int32_t>(
            *value),
        123);
}

TEST(
    CapabilityStorageTest,
    StoreAndFindDouble)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        12.5);

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_DOUBLE_EQ(
        std::any_cast<double>(
            *value),
        12.5);
}

TEST(
    CapabilityStorageTest,
    ReturnNullptrWhenNotFound)
{
    rim::CapabilityStorage storage;

    EXPECT_EQ(
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT),
        nullptr);
}

TEST(
    CapabilityStorageTest,
    OverwriteStoredValue)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        false);

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_TRUE(
        std::any_cast<bool>(
            *value));
}

TEST(
    CapabilityStorageTest,
    OverwriteWithDifferentType)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        int32_t{99});

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_EQ(
        value->type(),
        typeid(int32_t));

    EXPECT_EQ(
        std::any_cast<int32_t>(
            *value),
        99);
}

TEST(
    CapabilityStorageTest,
    StoredTypeIsPreserved)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        int32_t{10});

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_EQ(
        value->type(),
        typeid(int32_t));
}

TEST(
    CapabilityStorageTest,
    Exists)
{
    rim::CapabilityStorage storage;

    EXPECT_FALSE(
        storage.Exists(
            RI_CAPABILITY_ENVIRONMENT));

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    EXPECT_TRUE(
        storage.Exists(
            RI_CAPABILITY_ENVIRONMENT));
}

TEST(
    CapabilityStorageTest,
    EmptyStorageHasZeroSize)
{
    rim::CapabilityStorage storage;

    EXPECT_EQ(
        storage.Size(),
        0u);
}

TEST(
    CapabilityStorageTest,
    CountStoredCapabilities)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    storage.Store(
        RI_CAPABILITY_PRINT_READY,
        false);

    EXPECT_EQ(
        storage.Size(),
        2u);
}

TEST(
    CapabilityStorageTest,
    MultipleCapabilitiesCanBeStored)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    storage.Store(
        RI_CAPABILITY_PRINT_READY,
        false);

    const auto* environment =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    const auto* printReady =
        storage.Find(
            RI_CAPABILITY_PRINT_READY);

    ASSERT_NE(
        environment,
        nullptr);

    ASSERT_NE(
        printReady,
        nullptr);

    EXPECT_TRUE(
        std::any_cast<bool>(
            *environment));

    EXPECT_FALSE(
        std::any_cast<bool>(
            *printReady));
}

TEST(
    CapabilityStorageTest,
    StoreLargeObject)
{
    rim::CapabilityStorage storage;

    std::vector<uint8_t> buffer(
        4096,
        0xAA);

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        buffer);

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    const auto& stored =
        std::any_cast<
            const std::vector<uint8_t>&>(
            *value);

    ASSERT_EQ(
        stored.size(),
        4096u);

    EXPECT_EQ(
        stored.front(),
        0xAA);

    EXPECT_EQ(
        stored.back(),
        0xAA);
}

TEST(
    CapabilityStorageTest,
    TypeMismatchThrows)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    const auto* value =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        value,
        nullptr);

    EXPECT_THROW(
        (
            std::any_cast<int32_t>(
                *value)
        ),
        std::bad_any_cast);
}

TEST(
    CapabilityStorageTest,
    FindDoesNotCreateEntry)
{
    rim::CapabilityStorage storage;

    EXPECT_EQ(
        storage.Size(),
        0u);

    EXPECT_EQ(
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT),
        nullptr);

    EXPECT_EQ(
        storage.Size(),
        0u);
}

TEST(
    CapabilityStorageTest,
    OverwriteDoesNotIncreaseSize)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    EXPECT_EQ(
        storage.Size(),
        1u);

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        false);

    EXPECT_EQ(
        storage.Size(),
        1u);
}

TEST(
    CapabilityStorageTest,
    ExistsAfterOverwrite)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        false);

    EXPECT_TRUE(
        storage.Exists(
            RI_CAPABILITY_ENVIRONMENT));
}

TEST(
    CapabilityStorageTest,
    RepeatedFindReturnsSameValue)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        int32_t{123});

    const auto* first =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    const auto* second =
        storage.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        first,
        nullptr);

    ASSERT_NE(
        second,
        nullptr);

    EXPECT_EQ(
        std::any_cast<int32_t>(
            *first),
        123);

    EXPECT_EQ(
        std::any_cast<int32_t>(
            *second),
        123);
}

TEST(
    CapabilityStorageTest,
    EmptyStorageHasZeroBucketsOrMore)
{
    rim::CapabilityStorage storage;

    EXPECT_GE(
        storage.BucketCount(),
        0u);
}

TEST(
    CapabilityStorageTest,
    BucketCountIncreasesAfterInsert)
{
    rim::CapabilityStorage storage;

    const auto before =
        storage.BucketCount();

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    EXPECT_GE(
        storage.BucketCount(),
        before);
}

TEST(
    CapabilityStorageTest,
    LoadFactorIsNonNegative)
{
    rim::CapabilityStorage storage;

    EXPECT_GE(
        storage.LoadFactor(),
        0.0f);
}

TEST(
    CapabilityStorageTest,
    LoadFactorChangesAfterInsert)
{
    rim::CapabilityStorage storage;

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    EXPECT_GT(
        storage.LoadFactor(),
        0.0f);
}

TEST(
    CapabilityStorageTest,
    MaxLoadFactorIsPositive)
{
    rim::CapabilityStorage storage;

    EXPECT_GT(
        storage.MaxLoadFactor(),
        0.0f);
}

TEST(
    CapabilityStorageTest,
    LoadFactorIsZeroWhenEmpty)
{
    rim::CapabilityStorage storage;

    EXPECT_FLOAT_EQ(
        storage.LoadFactor(),
        0.0f);
}

TEST(
    CapabilityStorageTest,
    BucketCountDoesNotDecreaseAfterInsert)
{
    rim::CapabilityStorage storage;

    const auto before =
        storage.BucketCount();

    storage.Store(
        RI_CAPABILITY_ENVIRONMENT,
        true);

    const auto after =
        storage.BucketCount();

    EXPECT_GE(
        after,
        before);
}

} // namespace