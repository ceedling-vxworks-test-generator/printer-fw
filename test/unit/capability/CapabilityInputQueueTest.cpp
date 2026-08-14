#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityInputQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMValueFactory.hpp"
#include "ProductDefinition.hpp"
#include "PrinterAProductDefinition.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::CapabilityInputQueue queue;

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

    queue.Push(
        input);

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
    rim::CapabilityInputQueue queue;

    rim::CapabilityInput input{};

    input.changedDataId = 
            RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

    queue.Push(
        input);

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
    rim::CapabilityInputQueue queue;

    queue.Shutdown();

    rim::CapabilityInput input{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            input));
}