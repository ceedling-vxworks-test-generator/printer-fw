#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityWorker.hpp"

#include "CapabilityManager.hpp"
#include "CapabilityStore.hpp"

#include "PublisherInputQueue.hpp"

#include "PrinterAProductDefinition.hpp"

#include "CapabilityInput.hpp"

TEST(
    CapabilityWorkerTest,
    ExecuteOnce)
{
    rim::EventQueue<rim::CapabilityInput, rim::FifoPolicy> queue;

    rim::CapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

    rim::AddDoubleItem(
        input.snapshot,
        RI_DATA_TEMPERATURE_SENSOR_A,
        300.15);

    rim::AddDoubleItem(
        input.snapshot,
        RI_DATA_HUMIDITY_SENSOR,
        60.0);

    queue.Push(
        input);

    EXPECT_TRUE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    ReturnFalseWhenQueueEmpty)
{
    rim::EventQueue<rim::CapabilityInput, rim::CoalescingPolicy> queue;

    rim::CapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue);

    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    PublishDataAndCapabilityEvents)
{
    rim::EventQueue<rim::CapabilityInput, rim::CoalescingPolicy> queue;

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    rim::AddDoubleItem(
        input.snapshot,
        RI_DATA_TEMPERATURE_SENSOR_A,
        300.15);

    rim::AddDoubleItem(
        input.snapshot,
        RI_DATA_HUMIDITY_SENSOR,
        60.0);

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

    queue.Push(
        input);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::PublisherInput dataNotification{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            dataNotification));

    EXPECT_EQ(
        rim::NotificationTargetType::Data,
        dataNotification.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            RI_DATA_TEMPERATURE_SENSOR_A),
        dataNotification.target.id);

    rim::PublisherInput capabilityNotification{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            capabilityNotification));

    EXPECT_EQ(
        rim::NotificationTargetType::Capability,
        capabilityNotification.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            RI_CAPABILITY_ENVIRONMENT),
        capabilityNotification.target.id);
}