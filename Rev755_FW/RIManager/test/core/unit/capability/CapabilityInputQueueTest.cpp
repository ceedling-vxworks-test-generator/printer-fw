#include <gtest/gtest.h>

#include "CapabilityInput.hpp"
#include "Product.hpp"
#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"

TEST(
    CapabilityInputQueueTest,
    PushPop)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::CapabilityInput input{};

    input.changedId =
    {
        rim::RIMIdType::Data,
        RI_DATA_TEMPERATURE_SENSOR_A
    };

    ASSERT_TRUE(
        queue.Push(
            input));

    rim::CapabilityInput output{};

    ASSERT_TRUE(
        queue.TryPop(
            output));

    EXPECT_EQ(
        output.changedId,
        input.changedId);
}

TEST(
    CapabilityInputQueueTest,
    WaitAndPop)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::CapabilityInput input{};

    input.changedId =
    {
        rim::RIMIdType::Data,
        RI_DATA_TEMPERATURE_SENSOR_A
    };

    ASSERT_TRUE(
        queue.Push(
            input));

    rim::CapabilityInput output{};

    EXPECT_TRUE(
        queue.WaitAndPop(
            output));

    EXPECT_EQ(
        output.changedId,
        input.changedId);
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