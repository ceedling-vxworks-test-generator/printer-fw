#include <gtest/gtest.h>

#include "CapabilityChangeDetector.hpp"
#include "MachineCapabilityStore.hpp"

TEST(
    CapabilityChangeDetectorTest,
    DetectEnvironmentChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::EnvironmentCapability capability{};

    capability.temperature = 25.0;
    capability.humidity = 50.0;

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());

    EXPECT_EQ(
        "Environment",
        input.changedCapabilities.front());
}

TEST(
    CapabilityChangeDetectorTest,
    NoChangeReturnsFalse)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::EnvironmentCapability capability{};

    capability.temperature = 25.0;

    store.Store(
        capability);

    (void)detector.Detect(
        store);

    const auto input =
        detector.Detect(
            store);

    EXPECT_TRUE(
        input.changedCapabilities.empty());
}

TEST(
    CapabilityChangeDetectorTest,
    DetectErrorChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::ErrorCapability capability{};

    capability.errors.push_back(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());

    EXPECT_EQ(
        "Error",
        input.changedCapabilities.front());
}

TEST(
    CapabilityChangeDetectorTest,
    DetectPrintReadyChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::PrintReadyCapability capability{};

    capability.ready = true;

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        "PrintReady",
        input.changedCapabilities.front());

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());
}

TEST(
    CapabilityChangeDetectorTest,
    DetectConsumableChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::ConsumableCapability capability{};

    capability.tonerLevel = 80;
    capability.stapleLevel = 50;

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        "Consumable",
        input.changedCapabilities.front());

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());
}

TEST(
    CapabilityChangeDetectorTest,
    DetectJobChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::JobCapability capability{};

    capability.jobActive = true;
    capability.jobId = 123;

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        "Job",
        input.changedCapabilities.front());

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());
}

TEST(
    CapabilityChangeDetectorTest,
    DetectSecondEnvironmentChange)
{
    rim::MachineCapabilityStore store;

    rim::CapabilityChangeDetector detector;

    rim::EnvironmentCapability capability{};

    capability.temperature = 25.0;

    store.Store(
        capability);

    (void)detector.Detect(
        store);

    capability.temperature = 30.0;

    store.Store(
        capability);

    const auto input =
        detector.Detect(
            store);

    EXPECT_EQ(
        1U,
        input.changedCapabilities.size());

    EXPECT_EQ(
        "Environment",
        input.changedCapabilities.front());
}
