#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityInputQueue.hpp"
#include "RIMValueFactory.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::CapabilityInputQueue queue;

    rim::RIMSnapshot input{};

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperatureSensorA;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        input.items.PushBack(item);
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

        input.items.PushBack(item);
    }

    queue.Push(
        input);

    rim::RIMSnapshot output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output,
            rim::RIMDataId::kTemperatureSensorA),
        300.15);

    EXPECT_DOUBLE_EQ(
        GetDouble(
            output,
            rim::RIMDataId::kHumiditySensor),
        60.0);
}

TEST(
    CapabilityInputQueueTest,
    WaitAndPop)
{
    rim::CapabilityInputQueue queue;

    rim::RIMSnapshot input{};

    queue.Push(
        input);

    rim::RIMSnapshot output{};

    queue.WaitAndPop(
        output);
}

TEST(
    CapabilityInputQueueTest,
    Shutdown)
{
    rim::CapabilityInputQueue queue;

    queue.Shutdown();

    rim::RIMSnapshot snapshot{};

    EXPECT_FALSE(
        queue.WaitAndPop(
            snapshot));
}