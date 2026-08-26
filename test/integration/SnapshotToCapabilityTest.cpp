#include <gtest/gtest.h>

#include "printer_a.h"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValue.hpp"

#include "CapabilityItem/PrinterACapabilityBuilders.hpp"

namespace
{

void AddBoolItem(
    rim::RIMSnapshot& snapshot,
    RIDataId id,
    bool value)
{
    rim::RIMDataItem item{};

    item.id = id;

    item.value =
        rim::RIMValueFactory::CreateBool(
            value);

    snapshot.items.push_back(
        item);
}

} // namespace

TEST(
    SnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        snapshot.items.push_back(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        snapshot.items.push_back(
            item);
    }

    AddBoolItem(
        snapshot,
        RI_DATA_UPPER_DOOR_OPEN,
        false);

    AddBoolItem(
        snapshot,
        RI_DATA_RIGHT_DOOR_OPEN,
        false);

    AddBoolItem(
        snapshot,
        RI_DATA_LEFT_DOOR_OPEN,
        false);

    const auto environment =
        rim::BuildEnvironmentCapability(
            snapshot);

    const auto printReady =
        rim::BuildPrintReadyCapability(
            snapshot);

    EXPECT_EQ(
        environment.type,
        rim::ValueType::kInt32);

    EXPECT_EQ(
        environment.value.i32,
        1);

    EXPECT_EQ(
        printReady.type,
        rim::ValueType::kBool);

    EXPECT_TRUE(
        printReady.value.b);
}