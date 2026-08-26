#include <any>

#include <gtest/gtest.h>

#include "CapabilityManager.hpp"

#include "CapabilityStore.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

#include "PrinterAProductDefinition.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

namespace
{

rim::RIMDataItem CreateDoubleItem(
    RIDataId id,
    double value)
{
    rim::RIMDataItem item{};

    item.id =
        id;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            value);

    return item;
}

rim::RIMDataItem CreateBoolItem(
    RIDataId id,
    bool value)
{
    rim::RIMDataItem item{};

    item.id =
        id;

    item.value =
        rim::RIMValueFactory::CreateBool(
            value);

    return item;
}

}

TEST(
    CapabilityManagerTest,
    GenerateEnvironmentCapability)
{
    rim::CapabilityStore
        store;

    rim::CapabilityManager
        manager(
            store,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot
        snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            60.0));

    const auto changes =
        manager.Evaluate(
            snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A);

    ASSERT_EQ(
        1U,
        changes.changedCapabilities.size());

    EXPECT_EQ(
        RI_CAPABILITY_ENVIRONMENT,
        changes.changedCapabilities.front());

    const auto* capability =
        store.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        capability,
        nullptr);

    const auto& environment =
        std::any_cast<
            const rim::EnvironmentCapability&>(
                *capability);

    EXPECT_DOUBLE_EQ(
        300.15,
        environment.temperature);

    EXPECT_DOUBLE_EQ(
        60.0,
        environment.humidity);
}

TEST(
    CapabilityManagerTest,
    GeneratePrintReadyCapability)
{
    rim::CapabilityStore
        store;

    rim::CapabilityManager
        manager(
            store,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot
        snapshot;

    snapshot.items.push_back(
        CreateBoolItem(
            RI_DATA_UPPER_DOOR_OPEN,
            false));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_DATA_RIGHT_DOOR_OPEN,
            false));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_DATA_LEFT_DOOR_OPEN,
            false));

    const auto changes =
        manager.Evaluate(
            snapshot,
            RI_DATA_UPPER_DOOR_OPEN);

    ASSERT_EQ(
        1U,
        changes.changedCapabilities.size());

    EXPECT_EQ(
        RI_CAPABILITY_PRINT_READY,
        changes.changedCapabilities.front());

    const auto* capability =
        store.Find(
            RI_CAPABILITY_PRINT_READY);

    ASSERT_NE(
        capability,
        nullptr);

    const auto& printReady =
        std::any_cast<
            const rim::PrintReadyCapability&>(
                *capability);

    EXPECT_TRUE(
        printReady.ready);
}

TEST(
    CapabilityManagerTest,
    IgnoreUnknownData)
{
    rim::CapabilityStore
        store;

    rim::CapabilityManager
        manager(
            store,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot
        snapshot;

    const auto changes =
        manager.Evaluate(
            snapshot,
            RI_DATA_UNKNOWN);

    EXPECT_TRUE(
        changes.changedCapabilities.empty());
}

TEST(
    CapabilityManagerTest,
    SameEnvironmentValueDoesNotGenerateChange)
{
    rim::CapabilityStore
        store;

    rim::CapabilityManager
        manager(
            store,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot
        snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            60.0));

    const auto first =
        manager.Evaluate(
            snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A);

    ASSERT_FALSE(
        first.changedCapabilities.empty());

    const auto second =
        manager.Evaluate(
            snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_TRUE(
        second.changedCapabilities.empty());
}

TEST(
    CapabilityManagerTest,
    MissingHumidityDoesNotThrow)
{
    rim::CapabilityStore
        store;

    rim::CapabilityManager
        manager(
            store,
            rim::kPrinterAProductDefinition);

    rim::RIMSnapshot
        snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A));
}
