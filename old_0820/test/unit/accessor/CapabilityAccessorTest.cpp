#include <gtest/gtest.h>

#include "CapabilityAccessor.hpp"
#include "CapabilityStore.hpp"
#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

#include "printer_a.h"

namespace
{

class CapabilityAccessorTest :
    public ::testing::Test
{
protected:

    void StoreEnvironment(
        double temperature = 35.0,
        double humidity = 60.0)
    {
        rim::EnvironmentCapability env{};

        env.temperature =
            temperature;

        env.humidity =
            humidity;

        store.Store(
            RI_CAPABILITY_ENVIRONMENT,
            env);
    }

    rim::CapabilityStore store;
};

} // namespace

TEST_F(
    CapabilityAccessorTest,
    GetEnvironment)
{
    StoreEnvironment();

    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_DOUBLE_EQ(
        result.temperature,
        35.0);

    EXPECT_DOUBLE_EQ(
        result.humidity,
        60.0);
}

TEST_F(
    CapabilityAccessorTest,
    RepeatedReadReturnsSameValue)
{
    StoreEnvironment();

    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability first{};
    rim::EnvironmentCapability second{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            first));

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            second));

    EXPECT_DOUBLE_EQ(
        first.temperature,
        second.temperature);

    EXPECT_DOUBLE_EQ(
        first.humidity,
        second.humidity);
}

TEST_F(
    CapabilityAccessorTest,
    OverwriteCapability)
{
    StoreEnvironment(
        10.0,
        20.0);

    StoreEnvironment(
        30.0,
        40.0);

    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_DOUBLE_EQ(
        result.temperature,
        30.0);

    EXPECT_DOUBLE_EQ(
        result.humidity,
        40.0);
}

TEST_F(
    CapabilityAccessorTest,
    ReturnFalseWhenCapabilityNotFound)
{
    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));
}

TEST_F(
    CapabilityAccessorTest,
    ReturnFalseWhenCapabilityTypeMismatch)
{
    StoreEnvironment();

    rim::CapabilityAccessor accessor(
        store);

    int result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));
}

TEST_F(
    CapabilityAccessorTest,
    TypeMismatchStillDetectedAfterOverwrite)
{
    StoreEnvironment(
        10.0,
        20.0);

    StoreEnvironment(
        30.0,
        40.0);

    rim::CapabilityAccessor accessor(
        store);

    int result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));
}

TEST_F(
    CapabilityAccessorTest,
    EnvironmentCapabilityContainsExpectedValues)
{
    StoreEnvironment(
        123.45,
        67.89);

    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));

    EXPECT_DOUBLE_EQ(
        123.45,
        result.temperature);

    EXPECT_DOUBLE_EQ(
        67.89,
        result.humidity);
}

TEST_F(
    CapabilityAccessorTest,
    GetPrintReadyCapability)
{
    rim::PrintReadyCapability capability{};

    capability.ready =
        true;

    store.Store(
        RI_CAPABILITY_PRINT_READY,
        capability);

    rim::CapabilityAccessor accessor(
        store);

    rim::PrintReadyCapability result{};

    ASSERT_TRUE(
        accessor.TryGet(
            RI_CAPABILITY_PRINT_READY,
            result));

    EXPECT_TRUE(
        result.ready);
}

TEST_F(
    CapabilityAccessorTest,
    EnvironmentTypeMismatch)
{
    rim::PrintReadyCapability capability{};

    capability.ready =
        true;

    store.Store(
        RI_CAPABILITY_ENVIRONMENT,
        capability);

    rim::CapabilityAccessor accessor(
        store);

    rim::EnvironmentCapability result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_ENVIRONMENT,
            result));
}

TEST_F(
    CapabilityAccessorTest,
    PrintReadyTypeMismatch)
{
    rim::EnvironmentCapability capability{};

    store.Store(
        RI_CAPABILITY_PRINT_READY,
        capability);

    rim::CapabilityAccessor accessor(
        store);

    rim::PrintReadyCapability result{};

    EXPECT_FALSE(
        accessor.TryGet(
            RI_CAPABILITY_PRINT_READY,
            result));
}

