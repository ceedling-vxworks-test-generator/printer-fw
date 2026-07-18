#include <gtest/gtest.h>

#include "capability/MachineCapabilityStore.hpp"

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