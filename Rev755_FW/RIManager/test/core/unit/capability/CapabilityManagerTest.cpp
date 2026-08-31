#include <gtest/gtest.h>

#include "CapabilityManager.hpp"
#include "Product.hpp"
#include "ProductContext.hpp"
#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "test/core/support/RIMDataTestHelper.hpp"
#include "test/core/support/TestDomainInitializer.hpp"

namespace
{

rim::RIMDataItem FindCapability(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    const RICapabilityId capabilityId)
{
    const auto domainId =
        context.FindDomainId(
            {
                rim::RIMIdType::Capability,
                static_cast<std::uint32_t>(
                    capabilityId)
            });
            
    EXPECT_NE(
        domainId,
        rim::kInvalidDomainId);

    if (domainId == rim::kInvalidDomainId)
    {
        ADD_FAILURE()
            << "Capability domain was not found. capabilityId="
            << capabilityId;

        return {};
    }

    const auto* storage =
        store.Find(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    if (storage == nullptr)
    {
        ADD_FAILURE()
            << "Partition storage was not found. domainId="
            << domainId;

        return {};
    }

    rim::RIMDataItem item{};

    EXPECT_TRUE(
        storage->Find(
            static_cast<RIDataId>(
                capabilityId),
            item));

    return item;
}

} // namespace

using namespace rim::test;

TEST(
    CapabilityManagerTest,
    GenerateEnvironmentCapability)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kProductDefinition);

    rim::test::RegisterAllDomains(
        store,
        productContext);

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

    ASSERT_TRUE(
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

    rim::ProductContext productContext(
        rim::kProductDefinition);
 
    rim::test::RegisterAllDomains(
        store,
        productContext);

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

    ASSERT_TRUE(
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