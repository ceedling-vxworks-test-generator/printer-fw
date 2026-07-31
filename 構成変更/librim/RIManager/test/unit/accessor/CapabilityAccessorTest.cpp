#include <gtest/gtest.h>

#include "CapabilityAccessor.hpp"

#include "ErrorInfo.hpp"

TEST(
    CapabilityAccessorTest,
    GetEnvironment)
{
    rim::MachineCapabilityStore store;

    rim::EnvironmentCapability env{};

    env.temperature = 35.0;

    store.Store(
        env);

    rim::CapabilityAccessor accessor(
        store);

    const auto result =
        accessor.GetEnvironment();

    EXPECT_DOUBLE_EQ(
        result.temperature,
        35.0);
}

TEST(
    CapabilityAccessorTest,
    GetError)
{
    rim::MachineCapabilityStore
        store;

    rim::ErrorCapability
        capability{};

    capability.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    store.Store(
        capability);

    rim::CapabilityAccessor
        accessor(
            store);

    const auto result =
        accessor.GetError();

    ASSERT_EQ(
        result.errors.size(),
        1U);

    EXPECT_EQ(
        result.errors.front()
            .errorCode,
        1001U);
}

TEST(
    CapabilityAccessorTest,
    GetJob)
{
    rim::MachineCapabilityStore
        store;

    rim::JobCapability
        capability{};

    capability.jobActive = true;
    capability.jobId = 123;

    store.Store(
        capability);

    rim::CapabilityAccessor
        accessor(
            store);

    const auto result =
        accessor.GetJob();

    EXPECT_TRUE(
        result.jobActive);

    EXPECT_EQ(
        result.jobId,
        123);
}