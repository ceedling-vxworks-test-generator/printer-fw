#include <gtest/gtest.h>

#include "CapabilityAccessor.hpp"

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