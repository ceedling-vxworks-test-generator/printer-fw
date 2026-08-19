#include <gtest/gtest.h>

#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"
#include "RIMValueFactory.hpp"
#include "PrinterAProductDefinition.hpp"

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
    SnapshotToCapabilityTest,
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
                300.15);

        snapshot.items.push_back(item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kHumiditySensor;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        snapshot.items.push_back(item);
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

    rim::MachineCapabilityStore
        store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

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