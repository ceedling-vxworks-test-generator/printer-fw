#include <gtest/gtest.h>

#include "CapabilityManager.hpp"

#include "PrinterAProductDefinition.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "test/support/RIMDataTestHelper.hpp"
#include "ProductContext.hpp"

namespace
{

rim::RIMDataItem FindCapability(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    RICapabilityId capabilityId)
{
    const auto domainId =
        context.FindCapabilityDomainId(
            capabilityId);

    EXPECT_NE(
        domainId,
        rim::kInvalidDomainId);

    const auto* storage =
        store.Find(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    rim::RIMDataItem item{};

    EXPECT_TRUE(
        storage->Find(
            static_cast<RIDataId>(
                capabilityId),
            item));

    return item;
}

}

using namespace rim::test;

TEST(
    CapabilityManagerTest,
    GenerateEnvironmentCapability)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);

    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            60.0));

    const bool changed =
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentCapability);

    EXPECT_TRUE(
        changed);

    const auto item =
        FindCapability(
            store,
            productContext,
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        item.value.type,
        rim::ValueType::kInt32);

    EXPECT_EQ(
        item.value.value.i32,
        1);

}

TEST(
    CapabilityManagerTest,
    GeneratePrintReadyCapability)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);
    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

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

    const bool changed =
        manager.Evaluate(
            snapshot,
            &rim::kPrintReadyCapability);

    EXPECT_TRUE(
        changed);

    const auto item =
        FindCapability(
            store,
            productContext,
            RI_CAPABILITY_PRINT_READY);

    EXPECT_EQ(
        item.value.type,
        rim::ValueType::kBool);

    EXPECT_TRUE(
        item.value.value.b);
}

TEST(
    CapabilityManagerTest,
    SameEnvironmentValueDoesNotGenerateChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);
    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            60.0));

    const bool first =
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentCapability);

    EXPECT_TRUE(
        first);

    const auto second =
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentCapability);

    EXPECT_FALSE(
        second);
}

TEST(
    CapabilityManagerTest,
    MissingHumidityDoesNotThrow)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);

    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentCapability));
}

TEST(
    CapabilityManagerTest,
    HumidityChangesButCapabilityStateUnchanged)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);

    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot1;

    snapshot1.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot1.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            60.0));

    const bool first =
        manager.Evaluate(
            snapshot1,
            &rim::kEnvironmentCapability);

    EXPECT_TRUE(
        first);

    rim::RIMSnapshot snapshot2;

    snapshot2.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot2.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            70.0));

    const bool second =
        manager.Evaluate(
            snapshot2,
            &rim::kEnvironmentCapability);

    EXPECT_FALSE(
        second);
}

TEST(
    CapabilityManagerTest,
    EnvironmentStateChangesGeneratesChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext context(
        rim::kPrinterAProductDefinition);

    rim::CapabilityManager manager(
        store,
        context);

    rim::RIMSnapshot snapshot1;

    snapshot1.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            300.15));

    snapshot1.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            30.0));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot1,
            &rim::kEnvironmentCapability));

    rim::RIMSnapshot snapshot2;

    snapshot2.items.push_back(
        CreateDoubleItem(
            RI_DATA_TEMPERATURE_SENSOR_A,
            340.15));

    snapshot2.items.push_back(
        CreateDoubleItem(
            RI_DATA_HUMIDITY_SENSOR,
            80.0));

    const bool second =
        manager.Evaluate(
            snapshot2,
            &rim::kEnvironmentCapability);

    EXPECT_TRUE(
        second);

    const auto item =
        FindCapability(
            store,
            context,
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        2,
        item.value.value.i32);
}

TEST(
    CapabilityManagerTest,
    SamePrintReadyValueDoesNotGenerateChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext( rim::kPrinterAProductDefinition);
    rim::CapabilityManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

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

    const bool first =
        manager.Evaluate(
            snapshot,
            &rim::kPrintReadyCapability);

    EXPECT_TRUE(
        first);

    const bool second =
        manager.Evaluate(
            snapshot,
            &rim::kPrintReadyCapability);

    EXPECT_FALSE(
        second);

    const auto item =
        FindCapability(
            store,
            productContext,
            RI_CAPABILITY_PRINT_READY);

    EXPECT_EQ(
        item.value.type,
        rim::ValueType::kBool);

    EXPECT_TRUE(
        item.value.value.b);
}
