#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityInputQueue.hpp"
#include "CapabilityInput.hpp"
#include "RIMValueFactory.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::CapabilityInputQueue queue;

    rim::CapabilityInput input{};

    input.changedDataId =
        rim::RIMDataId::kTemperatureSensorA;

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperatureSensorA;

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
            rim::RIMDataId::kHumiditySensor;

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

    EXPECT_EQ(
        output.changedDataId,
        rim::RIMDataId::kTemperatureSensorA);

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output.snapshot,
            rim::RIMDataId::kTemperatureSensorA),
        300.15);

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output.snapshot,
            rim::RIMDataId::kHumiditySensor),
        60.0);
}

TEST(
    CapabilityInputQueueTest,
    WaitAndPop)
{
    rim::CapabilityInputQueue queue;

    rim::CapabilityInput input{};

    input.changedDataId =
        rim::RIMDataId::kTemperatureSensorA;

    queue.Push(
        input);

    rim::CapabilityInput output{};

    EXPECT_TRUE(
        queue.WaitAndPop(
            output));

    EXPECT_EQ(
        output.changedDataId,
        rim::RIMDataId::kTemperatureSensorA);
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