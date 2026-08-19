#include <gtest/gtest.h>

#include "CapabilityChangeDetector.hpp"
#include "MachineCapabilityStore.hpp"

TEST(
    CapabilityChangeDetectorTest,
    NoNotifyWhenSameValue)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::EnvironmentCapability capability{};

    capability.temperature = 300.15;
    capability.humidity = 60.0;

    store.Store(
        capability);

    const auto first =
        detector.Detect(
            store);

    EXPECT_EQ(
        1U,
        first.changedCapabilities.size());

    EXPECT_EQ(
        "Environment",
        first.changedCapabilities.front());

    const auto second =
        detector.Detect(
            store);

    EXPECT_TRUE(
        second.changedCapabilities.empty());
}