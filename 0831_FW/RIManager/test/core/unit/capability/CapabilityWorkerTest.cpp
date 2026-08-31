#include <gtest/gtest.h>

#include "test/core/support/CapabilityWorkerFixture.hpp"

#include "test/core/support/NotificationAssert.hpp"
#include "test/core/support/TestSupport.hpp"

#include "rim_api.h"

namespace
{

rim::RIMDataItem FindCapability(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    RICapabilityId capabilityId)
{
    const auto domainId =
        context.FindDomainId(
            rim::RIMId
            {
                rim::RIMIdType::Capability,
                static_cast<std::uint32_t>(
                    capabilityId)
            });

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

rim::RIMDataItem FindFacade(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    RIFacadeId facadeId)
{
    const auto domainId =
        context.FindDomainId(
            rim::RIMId
            {
                rim::RIMIdType::Facade,
                static_cast<std::uint32_t>(
                    facadeId)
            });

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
                facadeId),
            item));

    return item;
}

std::vector<rim::PublisherInput>
CollectNotifications(
    rim::PublisherInputQueue& queue)
{
    std::vector<rim::PublisherInput> result;

        rim::PublisherInput notification{};

        while (
            queue.TryPop(
                notification))
        {
            result.push_back(
                notification);
        }

        return result;
    }
}

bool HasFacadeNotification(
    const std::vector<rim::PublisherInput>& notifications,
    RIFacadeId facadeId)
{
    for (const auto& notification :
         notifications)
    {
        if ((notification.target.type ==
                rim::RIMIdType::Facade) &&
            (notification.target.id ==
                static_cast<std::uint32_t>(
                    facadeId)))
        {
            return true;
        }
    }

    return false;
}

TEST_F(
    CapabilityWorkerFixture,
    GenerateEnvironmentCapability)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    rim::RIMDataItem temperature{};

    temperature.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    temperature.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    storage.Store(
        temperature);

    rim::RIMDataItem humidity{};

    humidity.id =
        RI_DATA_HUMIDITY_SENSOR;

    humidity.value =
        rim::RIMValueFactory::CreateDouble(
            60.0);

    storage.Store(
        humidity);

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_ENVIRONMENT_READY));

    const auto capability =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        1,
        capability.value.value.i32);
}

TEST_F(
    CapabilityWorkerFixture,
    SameEnvironmentStateDoesNotNotifyAgain)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    rim::RIMDataItem temperature{};

    temperature.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    temperature.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    storage.Store(
        temperature);

    rim::RIMDataItem humidity{};

    humidity.id =
        RI_DATA_HUMIDITY_SENSOR;

    humidity.value =
        rim::RIMValueFactory::CreateDouble(
            60.0);

    storage.Store(
        humidity);

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_ENVIRONMENT_READY));

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectNoNotification(
        publisherQueue);
}

TEST_F(
    CapabilityWorkerFixture,
    ChangedEnvironmentStateNotifiesAgain)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    rim::RIMDataItem temperature{};

    temperature.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    temperature.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    storage.Store(
        temperature);

    rim::RIMDataItem humidity{};

    humidity.id =
        RI_DATA_HUMIDITY_SENSOR;

    humidity.value =
        rim::RIMValueFactory::CreateDouble(
            30.0);

    storage.Store(
        humidity);

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    temperature.value =
        rim::RIMValueFactory::CreateDouble(
            340.15);

    storage.Store(
        temperature);

    humidity.value =
        rim::RIMValueFactory::CreateDouble(
            80.0);

    storage.Store(
        humidity);

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    // Facade通知があっても失敗させない
    CollectNotifications(
        publisherQueue);

    const auto capability =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        2,
        capability.value.value.i32);
}

TEST_F(
    CapabilityWorkerFixture,
    GeneratePrintReadyCapability)
{
    auto& storage =
        GetPartitionStorage(
            2U);

    rim::test::TestSupport::
        SetAllDoorsClosed(
            storage);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_PRINT_READY);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_OPERATION_READY));

    const auto capability =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_PRINT_READY);

    EXPECT_TRUE(
        capability.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    ChangedPrintReadyStateNotifiesAgain)
{
    auto& storage =
        GetPartitionStorage(
            2U);

    rim::test::TestSupport::
        SetAllDoorsClosed(
            storage);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.value =
        rim::RIMValueFactory::CreateBool(
            true);

    storage.Store(
        item);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_PRINT_READY);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_OPERATION_READY));

    const auto capability =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_PRINT_READY);

    EXPECT_FALSE(
        capability.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    GenerateEnvironmentReadyFacade)
{
    auto& storage =
        GetPartitionStorage(
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
                30.0);

        storage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_ENVIRONMENT_READY));

    const auto facade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_TRUE(
        facade.value.value.b);

}

TEST_F(
    CapabilityWorkerFixture,
    GenerateOperationReadyFacade)
{
    auto& environmentStorage =
        GetPartitionStorage(
            1U);

    auto& doorStorage =
        GetPartitionStorage(
            2U);

    rim::test::TestSupport::
        SetAllDoorsClosed(
            doorStorage);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        environmentStorage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        environmentStorage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_PRINT_READY);

    //
    // Capability確認
    //
    const auto environment =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_ENVIRONMENT);

    EXPECT_EQ(
        1,
        environment.value.value.i32);

    const auto printReady =
        FindCapability(
            domainStore,
            productContext,
            RI_CAPABILITY_PRINT_READY);

    EXPECT_TRUE(
        printReady.value.value.b);

    //
    // 通知確認
    //
    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_OPERATION_READY));

    //
    // Facade確認
    //
    const auto facade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    EnvironmentReadyFacadeChangesWhenEnvironmentBecomesState2)
{
    auto& storage =
        GetPartitionStorage(
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
                30.0);

        storage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                340.15);

        storage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                80.0);

        storage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_ENVIRONMENT_READY));

    const auto facade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    OperationReadyFacadeChangesWhenPrintReadyBecomesFalse)
{
    auto& environmentStorage =
        GetPartitionStorage(
            1U);

    auto& doorStorage =
        GetPartitionStorage(
            2U);

    rim::test::TestSupport::
        SetAllDoorsClosed(
            doorStorage);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        environmentStorage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        environmentStorage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_UPPER_DOOR_OPEN;

        item.value =
            rim::RIMValueFactory::CreateBool(
                true);

        doorStorage.Store(
            item);
    }

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_PRINT_READY);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_OPERATION_READY));

    const auto facade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    SameOperationReadyFacadeValueDoesNotNotifyAgain)
{
    auto& environmentStorage =
        GetPartitionStorage(
            1U);

    auto& doorStorage =
        GetPartitionStorage(
            2U);

    rim::test::TestSupport::
        SetAllDoorsClosed(
            doorStorage);

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        environmentStorage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        environmentStorage.Store(
            item);
    }

    //
    // 初回生成
    //

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    //
    // 同じ状態で再評価
    //

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectNoNotification(
        publisherQueue);

    const auto facade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    OperationReadyFacadeChangesWhenEnvironmentBecomesNotReady)
{
    auto& environmentStorage =
        GetPartitionStorage(
            1U);

    auto& doorStorage =
        GetPartitionStorage(
            2U);

    //
    // PrintReady=true
    //
    rim::test::TestSupport::
        SetAllDoorsClosed(
            doorStorage);

    //
    // Environment=1
    //
    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        environmentStorage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                30.0);

        environmentStorage.Store(
            item);
    }

    //
    // OperationReady=true生成
    //
    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    Execute(
        RI_DATA_UPPER_DOOR_OPEN);

    const auto initialFacade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_TRUE(
        initialFacade.value.value.b);

    //
    // 通知クリア
    //
    rim::PublisherInput notification{};

    while (
        publisherQueue.TryPop(
            notification))
    {
    }

    //
    // Environment=2へ遷移
    //
    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                340.15);

        environmentStorage.Store(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            RI_DATA_HUMIDITY_SENSOR;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                80.0);

        environmentStorage.Store(
            item);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::test::ExpectDataNotification(
        publisherQueue);

    rim::test::ExpectCapabilityNotification(
        publisherQueue,
        RI_CAPABILITY_ENVIRONMENT);

    const auto notifications =
        CollectNotifications(
            publisherQueue);

    EXPECT_TRUE(
        HasFacadeNotification(
            notifications,
            RI_FACADE_OPERATION_READY));

    const auto updatedFacade =
        FindFacade(
            domainStore,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_FALSE(
        updatedFacade.value.value.b);
}

TEST_F(
    CapabilityWorkerFixture,
    ExecuteOnceReturnsFalseWhenQueueEmpty)
{
    EXPECT_FALSE(
        worker.ExecuteOnce());
}

TEST_F(
    CapabilityWorkerFixture,
    MissingRequiredDataDoesNotCrash)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    storage.Store(
        item);

    EXPECT_NO_THROW(
        Execute(
            RI_DATA_TEMPERATURE_SENSOR_A));
}

TEST_F(
    CapabilityWorkerFixture,
    NotificationOrderIsDataCapabilityFacade)
{
    auto& storage =
        GetPartitionStorage(
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

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput event{};

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::RIMIdType::Data,
        event.target.type);

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::RIMIdType::Capability,
        event.target.type);

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::RIMIdType::Facade,
        event.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            RI_FACADE_ENVIRONMENT_READY),
        event.target.id);

    ASSERT_TRUE(
        publisherQueue.TryPop(
            event));

    EXPECT_EQ(
        rim::RIMIdType::Facade,
        event.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            RI_FACADE_OPERATION_READY),
        event.target.id);

    EXPECT_FALSE(
        publisherQueue.TryPop(
            event));
}

TEST_F(
    CapabilityWorkerFixture,
    DataNotificationInheritsPublisherCompressionPolicy)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    rim::RIMDataItem temperature{};

    temperature.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    temperature.value =
        rim::RIMValueFactory::CreateDouble(
            300.15);

    storage.Store(
        temperature);

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    bool found = false;

    while (
        publisherQueue.TryPop(
            notification))
    {
        if (
            notification.target.type ==
                rim::RIMIdType::Data)
        {
            found = true;

            const auto* item =
                productContext.FindDataItem(
                    {
                        rim::RIMIdType::Data,
                        notification.target.id
                    });
                    
            ASSERT_NE(
                nullptr,
                item);

            EXPECT_EQ(
                item->publisherCompressionPolicy,
                notification.compressionPolicy);

            break;
        }
    }

    EXPECT_TRUE(
        found);
}

TEST_F(
    CapabilityWorkerFixture,
    CapabilityNotificationInheritsPublisherCompressionPolicy)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    {
        rim::RIMDataItem temperature{};

        temperature.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        temperature.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            temperature);
    }

    {
        rim::RIMDataItem humidity{};

        humidity.id =
            RI_DATA_HUMIDITY_SENSOR;

        humidity.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            humidity);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    bool found = false;

    while (
        publisherQueue.TryPop(
            notification))
    {
        if (
            notification.target.type ==
                rim::RIMIdType::Capability &&
            notification.target.id ==
                RI_CAPABILITY_ENVIRONMENT)
        {
            found = true;

            const auto* capability =
                productContext.FindCapability(
                    {
                        rim::RIMIdType::Capability,
                        notification.target.id
                    });

            ASSERT_NE(
                nullptr,
                capability);

            EXPECT_EQ(
                capability->publisherCompressionPolicy,
                notification.compressionPolicy);

            break;
        }
    }

    EXPECT_TRUE(
        found);
}

TEST_F(
    CapabilityWorkerFixture,
    FacadeNotificationInheritsPublisherCompressionPolicy)
{
    auto& storage =
        GetPartitionStorage(
            1U);

    {
        rim::RIMDataItem temperature{};

        temperature.id =
            RI_DATA_TEMPERATURE_SENSOR_A;

        temperature.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        storage.Store(
            temperature);
    }

    {
        rim::RIMDataItem humidity{};

        humidity.id =
            RI_DATA_HUMIDITY_SENSOR;

        humidity.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        storage.Store(
            humidity);
    }

    Execute(
        RI_DATA_TEMPERATURE_SENSOR_A);

    rim::PublisherInput notification{};

    bool found = false;

    while (
        publisherQueue.TryPop(
            notification))
    {
        if (
            notification.target.type ==
                rim::RIMIdType::Facade)
        {
            found = true;

            const auto* facade =
                productContext.FindFacade(
                    {
                        rim::RIMIdType::Facade,
                        notification.target.id
                    });

            ASSERT_NE(
                nullptr,
                facade);

            EXPECT_EQ(
                facade->publisherCompressionPolicy,
                notification.compressionPolicy);

            break;
        }
    }

    EXPECT_TRUE(
        found);
}