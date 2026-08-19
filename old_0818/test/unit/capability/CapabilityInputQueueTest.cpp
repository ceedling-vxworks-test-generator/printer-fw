#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityInput.hpp"
#include "RIMValueFactory.hpp"
#include "ProductDefinition.hpp"
#include "PrinterAProductDefinition.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::CapabilityInput input{};

    input.changedDataId = 
            RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        input.snapshot.items.push_back(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        input.snapshot.items.push_back(
            item);
    }

    ASSERT_TRUE(
        queue.Push(
            input));

    rim::CapabilityInput output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    ASSERT_NE(
        output.changedDataId,
        RI_DATA_UNKNOWN);

    EXPECT_EQ(
        output.changedDataId,
        RI_DATA_TEMPERATURE_SENSOR_A);

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output.snapshot,
            RI_DATA_TEMPERATURE_SENSOR_A),
        300.15);

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output.snapshot,
            RI_DATA_HUMIDITY_SENSOR),
        60.0);
}

TEST(
    CapabilityInputQueueTest,
    WaitAndPop)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::CapabilityInput input{};

    input.changedDataId = 
            RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

    ASSERT_TRUE(
        queue.Push(
            input));

    rim::CapabilityInput output{};

    EXPECT_TRUE(
        queue.WaitAndPop(
            output));

    ASSERT_NE(
        output.changedDataId,
        RI_DATA_UNKNOWN);

    EXPECT_EQ(
        output.changedDataId,
        RI_DATA_TEMPERATURE_SENSOR_A);
}

TEST(
    CapabilityInputQueueTest,
    Shutdown)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    queue.Shutdown();

    rim::CapabilityInput input{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            input));
}

TEST(
    CapabilityInputQueueTest,
    CoalescingKeepsLatestItem)
{
    rim::EventQueue<rim::RIMDataItem, rim::CoalescingPolicy> queue;

    rim::RIMDataItem oldItem{};
    oldItem.id =
        RI_DATA_UPPER_DOOR_OPEN;
    oldItem.value =
        rim::RIMValueFactory::CreateBool(
            false);

    rim::RIMDataItem newItem{};
    newItem.id =
        RI_DATA_UPPER_DOOR_OPEN;
    newItem.value =
        rim::RIMValueFactory::CreateBool(
            true);

    ASSERT_TRUE(
        queue.Push(
            oldItem));

    ASSERT_TRUE(
        queue.Push(
            newItem));

    rim::RIMDataItem result{};

    ASSERT_TRUE(
        queue.TryPop(
            result));

    bool value{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            result.value,
            value));

    EXPECT_TRUE(
        value);

    EXPECT_FALSE(
        queue.TryPop(
            result));
}
