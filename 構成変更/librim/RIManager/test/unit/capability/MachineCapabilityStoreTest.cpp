#include <gtest/gtest.h>

#include "ErrorInfo.hpp"
#include "MachineCapabilityStore.hpp"

TEST(
    MachineCapabilityStoreTest,
    StoreAndGetEnvironment)
{
    rim::MachineCapabilityStore store;

    rim::EnvironmentCapability capability{};

    capability.temperature =
        300.15;

    capability.humidity =
        60.0;

    store.Store(
        capability);

    EXPECT_DOUBLE_EQ(
        store.GetEnvironment().temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        store.GetEnvironment().humidity,
        60.0);
}

TEST(
    MachineCapabilityStoreTest,
    StoreAndGetConsumable)
{
    rim::MachineCapabilityStore store;

    rim::ConsumableCapability input{};

    input.stapleLevel = 80;

    store.Store(
        input);

    const auto& output =
        store.GetConsumable();

    EXPECT_EQ(
        output.stapleLevel,
        80);
}

TEST(
    MachineCapabilityStoreTest,
    StoreAndGetError)
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

    EXPECT_EQ(
        store.GetError()
            .errors.size(),
        1U);

    EXPECT_EQ(
        store.GetError()
            .errors.front()
            .errorCode,
        1001U);
}

TEST(
    MachineCapabilityStoreTest,
    StoreAndGetJob)
{
    rim::MachineCapabilityStore
        store;

    rim::JobCapability
        capability{};

    capability.jobActive = true;
    capability.jobId = 123;

    store.Store(
        capability);

    EXPECT_TRUE(
        store.GetJob()
            .jobActive);

    EXPECT_EQ(
        store.GetJob()
            .jobId,
        123);
}