#include <gtest/gtest.h>

#include "capability/CapabilityManager.hpp"
#include "capability/MachineCapabilityStore.hpp"

#include "capability/PrinterACapabilityRuleSet.hpp"

TEST(
    SnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    snapshot.temperature = 300.15;
    snapshot.humidity = 60.0;

    snapshot.upperDoorOpen = false;
    snapshot.rightDoorOpen = false;
    snapshot.leftDoorOpen = false;

    rim::MachineCapabilityStore store;

    rim::PrinterACapabilityRuleSet rules;

    rim::CapabilityManager manager(
        store,
        rules);

    manager.Evaluate(
        snapshot);

    EXPECT_DOUBLE_EQ(
        store.GetEnvironment().temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        store.GetEnvironment().humidity,
        60.0);

    EXPECT_TRUE(
        store.GetPrintReady().ready);
}