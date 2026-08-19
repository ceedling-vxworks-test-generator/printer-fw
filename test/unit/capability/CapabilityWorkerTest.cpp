#include <gtest/gtest.h>

#include <any>

#include "CapabilityWorker.hpp"

#include "CapabilityManager.hpp"
#include "CapabilityStore.hpp"

#include "PublisherInputQueue.hpp"

#include "PrinterAProductDefinition.hpp"

#include "CapabilityInput.hpp"

#include "DomainStorageRegistry.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

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

    rim::DomainStorageRegistry domainStore;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue,
        snapshotProvider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_NE(
        input.changedDataId,
        RI_DATA_UNKNOWN);

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

    rim::DomainStorageRegistry domainStore;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue,
        snapshotProvider);

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

    rim::DomainStorageRegistry domainStore;

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::CapabilityWorker worker(
        queue,
        manager,
        publisherQueue,
        snapshotProvider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

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

TEST(
    CapabilityWorkerTest,
    GenerateEnvironmentCapabilityFromStoredData)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            item);
    }

    rim::CapabilitySnapshotResolver
        snapshotResolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        snapshotProvider(
            snapshotResolver,
            snapshotAccessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            snapshotProvider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_TRUE(
        queue.Push(
            input));

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

TEST(
    CapabilityWorkerTest,
    StoreGeneratedEnvironmentCapability)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            item);
    }

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    const auto* capability =
        capabilityStore.Find(
            RI_CAPABILITY_ENVIRONMENT);

    ASSERT_NE(
        capability,
        nullptr);
}

TEST(
    CapabilityWorkerTest,
    GeneratePrintReadyCapability)
{
    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            2U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_UPPER_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_RIGHT_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_LEFT_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_UPPER_DOOR_OPEN;

    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    const auto* capability =
        capabilityStore.Find(
            RI_CAPABILITY_PRINT_READY);

    ASSERT_NE(
        capability,
        nullptr);
}

TEST(
    CapabilityWorkerTest,
    IgnoreUnknownDataDoesNotGenerateCapability)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_UNKNOWN;

    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::PublisherInput notification{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            notification));

    EXPECT_EQ(
        rim::NotificationTargetType::Data,
        notification.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            RI_DATA_UNKNOWN),
        notification.target.id);

    EXPECT_FALSE(
        publisherQueue.TryPop(
            notification));

    EXPECT_EQ(
        nullptr,
        capabilityStore.Find(
            RI_CAPABILITY_ENVIRONMENT));

    EXPECT_EQ(
        nullptr,
        capabilityStore.Find(
            RI_CAPABILITY_PRINT_READY));
}

TEST(
    CapabilityWorkerTest,
    EnvironmentCapabilityContainsExpectedValues)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            item);
    }

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    const auto* capability =
        capabilityStore.Find(
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
    CapabilityWorkerTest,
    PrintReadyCapabilityContainsExpectedValues)
{
    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            2U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_UPPER_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_RIGHT_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_LEFT_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                false);

        storage.Store(
            item);
    }

    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_UPPER_DOOR_OPEN;

    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    const auto* capability =
        capabilityStore.Find(
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
    CapabilityWorkerTest,
    MissingHumidityDoesNotCrash)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            item);
    }

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    ASSERT_TRUE(
        queue.Push(
            input));

    EXPECT_NO_THROW(
        worker.ExecuteOnce());
}

TEST(
    CapabilityWorkerTest,
    SameCapabilityValueDoesNotNotifyCapabilityAgain)
{
    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy> queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition);

    rim::DomainStorageRegistry
        domainStore;

    auto& storage =
        domainStore.GetOrCreate(
            1U);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            item);
    }

    rim::CapabilitySnapshotResolver
        resolver(
            rim::kPrinterAProductDefinition);

    rim::SnapshotAccessor
        accessor(
            domainStore,
            rim::kPrinterAProductDefinition);

    rim::CapabilitySnapshotProvider
        provider(
            resolver,
            accessor);

    rim::CapabilityWorker
        worker(
            queue,
            capabilityManager,
            publisherQueue,
            provider);

    rim::CapabilityInput input{};

    input.changedDataId =
        RI_DATA_TEMPERATURE_SENSOR_A;

    //
    // 1回目
    //
    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    rim::PublisherInput event{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::NotificationTargetType::Data,
        event.target.type);

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::NotificationTargetType::Capability,
        event.target.type);

    EXPECT_FALSE(
        publisherQueue.TryPop(
            event));

    //
    // 同じ値で再実行
    //
    ASSERT_TRUE(
        queue.Push(
            input));

    ASSERT_TRUE(
        worker.ExecuteOnce());

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::NotificationTargetType::Data,
        event.target.type);

    //
    // Capability通知は発生しない想定
    //
    EXPECT_FALSE(
        publisherQueue.TryPop(
            event));
}