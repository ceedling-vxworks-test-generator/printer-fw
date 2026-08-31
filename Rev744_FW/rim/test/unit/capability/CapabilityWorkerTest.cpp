#include <gtest/gtest.h>

#include "test/support/SnapshotTestHelper.hpp"

#include "CapabilityWorker.hpp"

#include "CapabilityManager.hpp"
#include "MachineCapabilityStore.hpp"

#include "PublisherInputQueue.hpp"

#include "PrinterAProductDefinition.hpp"

#include "CapabilityInput.hpp"

TEST(
    CapabilityWorkerTest,
    ExecuteOnce)
{
    rim::CapabilityInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        store,
        publisherQueue);

    rim::CapabilityInput input{};

    input.changedDataId =
        rim::RIMDataId::kTemperatureSensorA;

    rim::AddDoubleItem(
        input.snapshot,
        rim::RIMDataId::kTemperatureSensorA,
        300.15);

    rim::AddDoubleItem(
        input.snapshot,
        rim::RIMDataId::kHumiditySensor,
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
    rim::CapabilityInputQueue queue;

    rim::MachineCapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::PublisherInputQueue publisherQueue;

    rim::CapabilityWorker worker(
        queue,
        manager,
        store,
        publisherQueue);

    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    PublishEventGenerated)
{
    rim::CapabilityInputQueue queue;

    rim::PublisherInputQueue publisherQueue;

    rim::MachineCapabilityStore store;

    rim::CapabilityManager manager(
        store,
        rim::kPrinterAProductDefinition);

    rim::CapabilityWorker worker(
        queue,
        manager,
        store,
        publisherQueue);

    rim::CapabilityInput capabilityInput{};

    capabilityInput.changedDataId =
        rim::RIMDataId::kTemperatureSensorA;

    queue.Push(
        capabilityInput);

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::PublisherInput input{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            input));

    EXPECT_EQ(
        rim::NotificationTargetType::Data,
        input.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            rim::RIMDataId::kTemperatureSensorA),
        input.target.id);
}