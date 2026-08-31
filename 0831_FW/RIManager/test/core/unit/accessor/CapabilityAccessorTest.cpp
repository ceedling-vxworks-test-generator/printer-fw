#include <gtest/gtest.h>

#include "CapabilityAccessor.hpp"

#include "RIMValue.hpp"
#include "RIMValueFactory.hpp"

#include "rim_api.h"

#include "test/core/support/AccessorTestFixture.hpp"

using rim::test::AccessorTestFixture;

TEST_F(
    AccessorTestFixture,
    GetEnvironmentCapability)
{
    StoreEnvironment(1);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kInt32);

    EXPECT_EQ(
        result.value.i32,
        1);
}

TEST_F(
    AccessorTestFixture,
    GetPrintReadyCapability)
{
    StorePrintReady(true);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_PRINT_READY,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kBool);

    EXPECT_TRUE(
        result.value.b);
}

TEST_F(
    AccessorTestFixture,
    RepeatedReadReturnsSameValue)
{
    StoreEnvironment(1);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue first{};
    rim::RIMValue second{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            first));

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            second));

    EXPECT_EQ(
        first.type,
        second.type);

    EXPECT_EQ(
        first.value.i32,
        second.value.i32);
}

TEST_F(
    AccessorTestFixture,
    OverwriteCapability)
{
    StoreEnvironment(1);
    StoreEnvironment(2);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kInt32);

    EXPECT_EQ(
        result.value.i32,
        2);
}

TEST_F(
    AccessorTestFixture,
    ReturnFalseWhenCapabilityNotFound)
{
    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));
}

TEST_F(
    AccessorTestFixture,
    EnvironmentCapabilityStoresExpectedState)
{
    StoreEnvironment(2);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kInt32);

    EXPECT_EQ(
        result.value.i32,
        2);
}

TEST_F(
    AccessorTestFixture,
    PrintReadyCapabilityStoresExpectedState)
{
    StorePrintReady(false);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_PRINT_READY,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kBool);

    EXPECT_FALSE(
        result.value.b);
}

TEST_F(
    AccessorTestFixture,
    EnvironmentValueTypeIsInt32)
{
    StoreEnvironment(1);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kInt32);
}

TEST_F(
    AccessorTestFixture,
    PrintReadyValueTypeIsBool)
{
    StorePrintReady(true);

    rim::CapabilityAccessor accessor(
        domainStore,
        productContext);

    rim::RIMValue result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_PRINT_READY,
            result));

    EXPECT_EQ(
        result.type,
        rim::ValueType::kBool);
}