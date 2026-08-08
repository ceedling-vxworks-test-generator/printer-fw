#include "rim_api.h"

#include <memory>
#include <unordered_map>
#include <cstring>
#include <exception>

#include "CapabilityStore.hpp"

#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "ValueStore.hpp"
#include "RIMSnapshotManager.hpp"
#include "DataStoreWorker.hpp"
#include "RIMValueFactory.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "CapabilityInputQueue.hpp"
#include "CapabilityManager.hpp"
#include "CapabilityWorker.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"
#include "PublishManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "ChangeNotifyManager.hpp"

#include "PrinterAProductDefinition.hpp"

#include "NotificationReceiver.hpp"
#include "IProductProvider.hpp"
#include "RouteProvider.hpp"
#include "ProductFactory.hpp"
#include "RoutePipeline.hpp"

namespace
{

struct RIManagerContext
{
    std::unique_ptr<rim::IProductProvider>
    productProvider;

    rim::IChangeChecker*
        changeChecker {};

    std::vector<std::unique_ptr<rim::RoutePipeline>> pipelines;

    //
    // Queue
    //

    rim::StoreInputQueue
        storeQueue;

    rim::CapabilityInputQueue
        capabilityQueue;

    rim::PublisherInputQueue
        publisherQueue;

    //
    // Datastore
    //

    rim::ValueStore
        valueStore;

    rim::RIMSnapshotManager
        snapshotReader;

    rim::RouteProvider
        routeProvider;

    //
    // Capability
    //

    rim::CapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager;

    //
    // Publisher
    //
    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager
        notifyManager;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    rim::PublishManager
        publishManager;

    rim::NotificationReceiver
        receiver;

    rim::SubscriptionStore
        subscriptionStore;

    rim::SubscriberMailboxManager
        mailboxManager;

    //
    // Worker
    //

    rim::DataStoreWorker
        dataStoreWorker;

    rim::CapabilityWorker
        capabilityWorker;

    rim::PublisherWorker
        publisherWorker;

    //
    // Accessor
    //

    rim::CapabilityAccessor
        capabilityAccessor;

    RIManagerContext()
        : snapshotReader(
            valueStore)
        , routeProvider()
        , capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition)
        , notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry)
        , receiver(
            mailboxManager)
        , publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore)
        , dataStoreWorker(
            rim::kPrinterAProductDefinition,
            storeQueue,
            valueStore,
            snapshotReader,
            routeProvider)
        , capabilityWorker(
            capabilityQueue,
            capabilityManager,
            publisherQueue)
        , publisherWorker(
            publisherQueue,
            publishManager)
        , capabilityAccessor(
            capabilityStore)
    {
    }

    void Initialize(std::unique_ptr<rim::IProductProvider> provider)
    {
        productProvider = std::move(provider);
        routeProvider.Initialize(productProvider->GetProfile().definition);

        for (const auto& route : routeProvider.GetQueues())
        {
            pipelines.push_back(
                std::make_unique<rim::RoutePipeline>(
                    productProvider->GetProfile().definition,
                    *route.second,
                    valueStore,
                    productProvider->GetChangeChecker(),
                    snapshotReader,
                    capabilityManager,
                    publisherQueue));
        }
    }

};

std::unique_ptr<RIManagerContext>
    g_context;


}


extern "C"
{


int RIM_Create(void)
{
    if (g_context)
    {
        return RI_SUCCESS;
    }

    g_context =
        std::make_unique<
            RIManagerContext>();
	g_context->Initialize(rim::CreatePrinterAProvider());

    return RI_SUCCESS;
}

int RIM_Destroy(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    g_context->publisherWorker.Stop();

    // g_context->capabilityWorker.Stop();
    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Stop();
    }

    g_context->dataStoreWorker.Stop();

    g_context.reset();

    return RI_SUCCESS;
}

int RIM_Start(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    g_context->dataStoreWorker.Run();
    // g_context->capabilityWorker.Run();
    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Start();
    }

    g_context->publisherWorker.Run();

    return RI_SUCCESS;
}

int RIM_Stop(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    g_context->publisherWorker.Stop();

    // g_context->capabilityWorker.Stop();
    
    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Stop();
    }

    g_context->dataStoreWorker.Stop();

    return RI_SUCCESS;
}

RIStatus
RIM_GetCapability(
    RICapabilityId capabilityId,
    void* capability)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    if (capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    try
    {
        switch (capabilityId)
        {
        case RI_CAPABILITY_ENVIRONMENT:

            *static_cast<
                rim::EnvironmentCapability*>(
                    capability)
                =
                g_context->capabilityAccessor
                    .Get<
                        rim::EnvironmentCapability>(
                            RI_CAPABILITY_ENVIRONMENT);

            return RI_SUCCESS;

        case RI_CAPABILITY_PRINT_READY:

            *static_cast<
                rim::PrintReadyCapability*>(
                    capability)
                =
                g_context->capabilityAccessor
                    .Get<
                        rim::PrintReadyCapability>(
                            RI_CAPABILITY_PRINT_READY);

            return RI_SUCCESS;
        }
    }
    catch (const std::exception&)
    {
        return RI_NO_DATA;
    }

    return RI_INVALID_PARAMETER;
}

RIStatus
RIM_SubscribeCapability(
    RICapabilityId capabilityId,
    RINotificationCallback callback,
    uint64_t* subscriptionId)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    if (callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    RI_SUBSCRIPTION_REQUEST request{};


    request.targetType =RI_TARGET_CAPABILITY;
    request.targetId =static_cast<uint32_t>(capabilityId);
    request.trigger = RI_TRIGGER_ON_CHANGE;
    request.method = RI_METHOD_CALLBACK;
    request.intervalMs = 0;
    request.callback = callback;

    return static_cast<RIStatus>(
        RIM_Subscribe(
            &request,
            subscriptionId));
}

RIStatus RIM_Subscribe(
    const RI_SUBSCRIPTION_REQUEST* request,
    uint64_t* subscriptionId)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }
    if (request == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    const auto id =
        g_context->subscriptionStore
            .CreateSubscriptionId();

    rim::SubscriptionInfo info{};

    info.id = id;

    info.target.type =
        static_cast<
            rim::NotificationTargetType>(
                request->targetType);

    info.target.id =
        request->targetId;

    info.method =
        static_cast<
            rim::DeliveryMethod>(
                request->method);

    info.trigger =
        static_cast<
            rim::NotificationTrigger>(
                request->trigger);

    g_context->subscriptionStore.Register(
        info);
    if (info.trigger ==
        rim::NotificationTrigger::Periodic)
    {
        rim::PeriodicCondition condition{};

        condition.subscriptionId =id;

        condition.interval =
            std::chrono::milliseconds(
                request->intervalMs);

        condition.nextNotifyTime =
            std::chrono::steady_clock::now()
            + condition.interval;

        g_context->periodicNotifyManager
            .Register(
                condition);
    }

    if (request->method ==RI_METHOD_CALLBACK)
    {
        g_context->callbackRegistry.Subscribe(
            id,
            [callback = request->callback]
            (
                rim::SubscriptionId subscriptionId,
                const rim::NotificationMessage& message)
            {
                callback(
                    subscriptionId,
                    &message);
            });
    }

    *subscriptionId = id;

    return RI_SUCCESS;
}

RIStatus RIM_GetNotification(
    uint64_t subscriptionId,
    rim_notification_message_t* notificationMessage)
{
    if (notificationMessage == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    rim::NotificationMessage message{};

    if (!g_context->receiver.TryGetNotification(
            subscriptionId,
            message))
    {
        return RI_NO_DATA;
    }

    notificationMessage->targetType =
        static_cast<RI_TARGET_TYPE>(
            message.target.type);

    notificationMessage->targetId =
        message.target.id;

    notificationMessage->trigger =
        static_cast<RI_NOTIFICATION_TRIGGER>(
            message.trigger);

    return RI_SUCCESS;
}

uint32_t RIM_GetMailboxCount(
    uint64_t subscriptionId)
{
    if (!g_context)
    {
        return 0U;
    }

    return g_context->receiver.GetMailboxCount(subscriptionId);
}

int RIM_Unsubscribe(
    uint64_t subscriptionId)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    bool removed = false;

    removed |=
        g_context->callbackRegistry
            .Unsubscribe(
                subscriptionId);

    removed |=
        g_context->subscriptionStore
            .Remove(
                subscriptionId);

    removed |=
        g_context->periodicNotifyManager
            .Remove(
                subscriptionId);
                
    removed |=
    g_context->mailboxManager
        .RemoveMailbox(
            subscriptionId);

    return removed
        ? RI_SUCCESS
        : RI_NO_DATA;
}

int RIM_TestInjectTemperature(
    double temperature)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_TEMPERATURE_SENSOR_A;

    item.valueType =
        rim::ValueType::kDouble;

    item.value =
        rim::RIMValueFactory::
            CreateDouble(
                temperature);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectUpperDoorOpen(
    int opened)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_UPPER_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectRightDoorOpen(
    int opened)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_RIGHT_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectLeftDoorOpen(
    int opened)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_LEFT_DOOR_OPEN;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectStapleLevel(
    std::int32_t level)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_STAPLE_LEVEL;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::
            CreateInt32(
                level);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectJobActive(
    int active)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_JOB_ACTIVE;

    item.value =
        rim::RIMValueFactory::CreateBool(
            active != 0);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIM_TestInjectJobId(
    int jobId)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        RI_DATA_JOB_ID;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            jobId);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}






RIStatus
RIM_GetBool(
    RIDataId dataId,
    int* value)
{
    if (value == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    if (!g_context->valueStore.Find(
            dataId,
            item))
    {
        return RI_NO_DATA;
    }

    bool found{};

    if (!rim::RIMValueAccessor::GetBool(
            item.value,
            found))
    {
        return RI_INTERNAL_ERROR;
    }

    *value =
        found ? 1 : 0;

    return RI_SUCCESS;
}

RIStatus
RIM_SetBool(
    RIDataId dataId,
    int value)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        dataId;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                value != 0);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

RIStatus
RIM_GetInt32(
    RIDataId dataId,
    int32_t* value)
{
    if (value == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    if (!g_context->valueStore.Find(
            dataId,
            item))
    {
        return RI_NO_DATA;
    }

    int32_t found{};

    if (!rim::RIMValueAccessor::GetInt32(
            item.value,
            found))
    {
        return RI_INTERNAL_ERROR;
    }

    *value = found;

    return RI_SUCCESS;
}

RIStatus
RIM_SetInt32(
    RIDataId dataId,
    int32_t value)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        dataId;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::
            CreateInt32(
                value);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

RIStatus
RIM_GetDouble(
    RIDataId dataId,
    double* value)
{
    if (value == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    if (!g_context->valueStore.Find(
            dataId,
            item))
    {
        return RI_NO_DATA;
    }

    double found{};

    if (!rim::RIMValueAccessor::GetDouble(
            item.value,
            found))
    {
        return RI_INTERNAL_ERROR;
    }

    *value = found;

    return RI_SUCCESS;
}

RIStatus
RIM_SetDouble(
    RIDataId dataId,
    double value)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMDataItem item{};

    item.id =
        dataId;

    item.valueType =
        rim::ValueType::kDouble;

    item.value =
        rim::RIMValueFactory::
            CreateDouble(
                value);

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

RIStatus
RIM_GetBinary(
    RIDataId dataId,
    RI_BINARY* binary)
{
    if (binary == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::BinaryStoreValue* value{};

    if (!g_context->valueStore.FindBinary(
            dataId,
            value))
    {
        return RI_NO_DATA;
    }

    binary->data =
        value->data.data();

    binary->size =
        static_cast<uint32_t>(
            value->data.size());

    return RI_SUCCESS;
}

int RIM_SetBinary(
    RIDataId dataId,
    const void* data,
    size_t size)
{
    if (data == nullptr ||
        size == 0U)
    {
        return RI_INVALID_PARAMETER;
    }

    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    auto binary =
        std::make_unique<
            rim::BinaryStoreValue>();

    binary->data.resize(
        size);

    std::memcpy(
        binary->data.data(),
        data,
        size);

    rim::RIMDataItem item{};

    item.id =
        dataId;

    item.valueType =
        rim::ValueType::kBinary;

    item.value =
        rim::RIMValueFactory::
            CreateBinary(
                binary.release());

    g_context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

RIStatus
RIM_SetErrorList(
    const RI_FAULT_INFO_LIST* list)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    if (list == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    RI_BINARY binary{};

    binary.data =
        reinterpret_cast<const std::uint8_t*>(
            list);

    binary.size =
        sizeof(RI_FAULT_INFO_LIST);

    return static_cast<RIStatus>(
        RIM_SetBinary(
            RI_DATA_ERROR_LIST,
            list,
            sizeof(RI_FAULT_INFO_LIST)));
}

RIStatus
RIM_GetErrorList(
    RI_FAULT_INFO_LIST* list)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    if (list == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    RI_BINARY binary{};

    const auto result =
        RIM_GetBinary(
            RI_DATA_ERROR_LIST,
            &binary);

    if (result != RI_SUCCESS)
    {
        return result;
    }

    if (binary.size !=
        sizeof(RI_FAULT_INFO_LIST))
    {
        return RI_INTERNAL_ERROR;
    }

    std::memcpy(
        list,
        binary.data,
        sizeof(RI_FAULT_INFO_LIST));

    return RI_SUCCESS;
}


}