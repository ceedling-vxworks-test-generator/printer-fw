#include <gtest/gtest.h>

#include <any>

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "printer_a.h"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

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

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::CreateBool(
            value);

    snapshot.items.push_back(
        item);
}

} // namespace

TEST(
    EndToEndSnapshotToCapabilityTest,
    GenerateCapabilities)
{
    rim::RIMSnapshot snapshot{};

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        // item.valueType =
        //     rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        snapshot.items.push_back(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        // item.valueType =
        //     rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                40.0);

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
        std::any_cast<
            rim::EnvironmentCapability>(
                rim::BuildEnvironmentCapability(
                    snapshot));

    const auto printReady =
        std::any_cast<
            rim::PrintReadyCapability>(
                rim::BuildPrintReadyCapability(
                    snapshot));

    EXPECT_DOUBLE_EQ(
        environment.temperature,
        30.0);

    EXPECT_DOUBLE_EQ(
        environment.humidity,
        40.0);

    EXPECT_TRUE(
        printReady.ready);
}