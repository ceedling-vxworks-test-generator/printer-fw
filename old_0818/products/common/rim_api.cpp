#include "rim_api.h"

#include <memory>
#include <unordered_map>
#include <cstring>

#include "AdapterDispatcher.hpp"

#include "CapabilityStore.hpp"

#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "RIMSnapshotManager.hpp"
#include "DataStoreWorker.hpp"
#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"

#include "CapabilityManager.hpp"

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
#include "CallbackWorker.hpp"
#include "CallbackQueue.hpp"


namespace
{

struct RIManagerContext
{
    std::unique_ptr<rim::IProductProvider>
    productProvider;

    std::vector<std::unique_ptr<rim::RoutePipeline>> pipelines;

    //
    // Queue
    //

    rim::StoreInputQueue
        storeQueue;

    rim::PublisherInputQueue
        publisherQueue;

    //
    // Adapter
    //

    rim::AdapterDispatcher
        dispatcher;

    //
    // Datastore
    //

    rim::DomainStorageRegistry
        domainStore;

    rim::RIMSnapshotManager
        snapshotManager;

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

    rim::CallbackQueue
        callbackQueue;

    rim::CallbackWorker
        callbackWorker;

    //
    // Worker
    //

    rim::DataStoreWorker
        dataStoreWorker;

    rim::PublisherWorker
        publisherWorker;

    //
    // Accessor
    //

    rim::CapabilityAccessor
        capabilityAccessor;

    RIManagerContext()
        : dispatcher(
            rim::kPrinterAProductDefinition,
            storeQueue)
        , snapshotManager(
            domainStore)
        , routeProvider()
        , capabilityManager(
            capabilityStore,
            rim::kPrinterAProductDefinition)
        ,callbackWorker(
            callbackQueue,
            callbackRegistry)
        , notifyManager(
            subscriptionStore,
            mailboxManager,
            callbackRegistry,
            callbackQueue)
        , receiver(
            mailboxManager)
        , publishManager(
            notifyManager,
            periodicNotifyManager,
            subscriptionStore,
            rim::kPrinterAProductDefinition)
        , dataStoreWorker(
            rim::kPrinterAProductDefinition,
            storeQueue,
            domainStore,
            snapshotManager,
            routeProvider)
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
        const auto& product = productProvider->GetProfile().definition;

        routeProvider.Initialize(product);
        pipelines.clear();

        for (const auto& [name, queues]: routeProvider.GetQueues())
        {
            pipelines.push_back(
                std::make_unique<rim::RoutePipeline>(
                    product,
                    queues,
                    domainStore,
                    snapshotManager,
                    capabilityManager,
                    publisherQueue));
        }
    }

};

std::unique_ptr<RIManagerContext>
    g_context;


}

namespace
{

bool TryReadDataItem(
    RIDataId dataId,
    rim::RIMDataItem& item)
{
    if (!g_context)
    {
        return false;
    }

    const auto snapshot =
        g_context->snapshotManager.Read();

    return snapshot.Find(
        dataId,
        item);
}

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
    g_context->callbackWorker.Stop();

    // g_context->capabilityWorker.Stop();
    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Stop();
    }

    g_context->pipelines.clear();

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

    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Start();
    }
    g_context->callbackWorker.Run();
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
    g_context->callbackWorker.Stop();
    
    for (auto& pipeline : g_context->pipelines)
    {
        pipeline->Stop();
    }

    g_context->dataStoreWorker.Stop();

    return RI_SUCCESS;
}

rim::CapabilityAccessor*
RIM_GetCapabilityAccessor()
{
    if (!g_context)
    {
        return nullptr;
    }

    return &g_context->capabilityAccessor;
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

    if (!TryReadDataItem(
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

    if (!TryReadDataItem(
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

    return g_context->dispatcher.Dispatch(item)
           ? RI_SUCCESS
           : RI_INVALID_PARAMETER;
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

    if (!TryReadDataItem(
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

    rim::RIMDataItem item{};

    if (!TryReadDataItem(
            dataId,
            item))
    {
        return RI_NO_DATA;
    }

    const rim::BinaryStoreValue* value{};

    if (!rim::RIMValueAccessor::GetBinary(
            item.value,
            value))
    {
        return RI_INTERNAL_ERROR;
    }

    if (value == nullptr)
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

}