#include <gtest/gtest.h>

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "MachineCapabilityStore.hpp"
#include "CapabilityItem/PrinterACapabilityBuilders.hpp"

namespace
{

void AddBoolItem(
    rim::RIMSnapshot& snapshot,
    rim::RIMDataId id,
    bool value)
{
    rim::RIMDataItem item{};

    item.id = id;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            value);

    snapshot.items.push_back(
        item);
}

}

TEST(
    EndToEndSnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperatureSensorA;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        snapshot.items.push_back(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kHumiditySensor;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                40.0);

        snapshot.items.push_back(
            item);
    }

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kUpperDoorOpen,
        false);

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kRightDoorOpen,
        false);

    AddBoolItem(
        snapshot,
        rim::RIMDataId::kLeftDoorOpen,
        false);

    rim::MachineCapabilityStore store;

    rim::BuildEnvironmentCapability(
        snapshot,
        store);

    rim::BuildPrintReadyCapability(
        snapshot,
        store);

    const auto& env =
        store.GetEnvironment();

    const auto& ready =
        store.GetPrintReady();

    EXPECT_DOUBLE_EQ(
        env.temperature,
        30.0);

    EXPECT_DOUBLE_EQ(
        env.humidity,
        40.0);

    EXPECT_TRUE(
        ready.ready);
}