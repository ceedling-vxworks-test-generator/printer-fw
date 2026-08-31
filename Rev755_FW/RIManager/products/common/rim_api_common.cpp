#include "rim_api_common.h"

#include <memory>

#include "CoreVersionInfo.hpp"
#include "ProductVersionInfo.hpp"

#include "AdapterDispatcher.hpp"

#include "CapabilityAccessor.hpp"
#include "FacadeAccessor.hpp"

#include "RIMValue.hpp"
#include "RIMValueFactory.hpp"
#include "PartitionStorageRegistry.hpp"

#include "CapabilityManager.hpp"
#include "FacadeManager.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"
#include "PublishManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "ChangeNotifyManager.hpp"

#include "Product.hpp"

#include "NotificationReceiver.hpp"
#include "IProductProvider.hpp"
#include "RouteProvider.hpp"
#include "ProductFactory.hpp"
#include "RouteExecutor.hpp"
#include "CallbackWorker.hpp"
#include "CallbackQueue.hpp"

#include "SnapshotAccessor.hpp"
#include "CapabilitySnapshotProvider.hpp"
#include "FacadeSnapshotProvider.hpp"
#include "ProductContext.hpp"
namespace
{

struct RIManagerContext
{
    std::unique_ptr<rim::IProductProvider>
    productProvider;

    std::vector<std::unique_ptr<rim::RouteExecutor>> routeExecutor;
    rim::RouteProvider routeProvider;

    std::unique_ptr<rim::ProductContext> productContext;

    //
    // Queue
    //

    rim::PublisherInputQueue
        publisherQueue;

    //
    // Adapter
    //

    std::unique_ptr<
        rim::AdapterDispatcher>
            dispatcher;

    //
    // Datastore
    //

    rim::PartitionStorageRegistry
        domainStore;

    //
    // Capability
    //

    std::unique_ptr<
        rim::CapabilityManager>
            capabilityManager;

    std::unique_ptr<
        rim::CapabilitySnapshotResolver>
            capabilitySnapshotResolver;

    std::unique_ptr<rim::FacadeManager>
        facadeManager;

    //
    // Publisher
    //
    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager
        notifyManager;

    rim::PeriodicNotifyManager
        periodicNotifyManager;

    std::unique_ptr<
        rim::PublishManager>
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

    std::unique_ptr<
        rim::PublisherWorker>
            publisherWorker;

    //
    // Accessor
    //

    std::unique_ptr<
        rim::SnapshotAccessor>
            snapshotAccessor;

    std::unique_ptr<
        rim::CapabilitySnapshotProvider>
            capabilitySnapshotProvider;

    std::unique_ptr<
        rim::CapabilityAccessor>
            capabilityAccessor;

    std::unique_ptr<
        rim::FacadeSnapshotProvider>
            facadeSnapshotProvider;

    std::unique_ptr<
        rim::FacadeAccessor>
            facadeAccessor;

RIManagerContext()
    : routeProvider()
    , callbackWorker(
        callbackQueue,
        callbackRegistry)
    , notifyManager(
        subscriptionStore,
        mailboxManager,
        callbackRegistry,
        callbackQueue)
    , receiver(
        mailboxManager)
{
}

    void Initialize(
        std::unique_ptr<rim::IProductProvider> provider)
    {
        productProvider =
            std::move(provider);

        const auto& product =
            productProvider->GetProfile().definition;

        productContext =
            std::make_unique<
                rim::ProductContext>(
                    product);

        //
        // Data Domain
        //
        for (std::size_t i = 0;
            i < rim::GetDomainCount(product);
            ++i)
        {
            domainStore.RegisterDomain(
                static_cast<rim::DomainId>(
                    i + 1));
        }

        //
        // Capability Domain
        //
        for (const auto domainId :
            productContext->GetCapabilityDomainIds())
        {
            domainStore.RegisterDomain(
                domainId);
        }

        //
        // Facade Domain
        //
        for (const auto domainId :
            productContext->GetFacadeDomainIds())
        {
            domainStore.RegisterDomain(
                domainId);
        }

        facadeManager =
            std::make_unique<
                rim::FacadeManager>(
                    domainStore,
                    *productContext);

        capabilitySnapshotResolver =
            std::make_unique<
                rim::CapabilitySnapshotResolver>(
                    *productContext);

        snapshotAccessor =
            std::make_unique<
                rim::SnapshotAccessor>(
                    domainStore,
                    *productContext);

        capabilitySnapshotProvider =
            std::make_unique<
                rim::CapabilitySnapshotProvider>(
                    *capabilitySnapshotResolver,
                    *snapshotAccessor);

        capabilityManager =
            std::make_unique<
                rim::CapabilityManager>(
                    domainStore,
                    *productContext);

        facadeSnapshotProvider =
            std::make_unique<
                rim::FacadeSnapshotProvider>(
                    *productContext,
                    *snapshotAccessor);

        capabilityAccessor =
            std::make_unique<
                rim::CapabilityAccessor>(
                    domainStore,
                    *productContext);

        facadeAccessor =
            std::make_unique<
                rim::FacadeAccessor>(
                    domainStore,
                    *productContext);

        dispatcher =
            std::make_unique<
                rim::AdapterDispatcher>(
                    *productContext,
                    routeProvider);

        publishManager =
            std::make_unique<
                rim::PublishManager>(
                    notifyManager,
                    periodicNotifyManager,
                    subscriptionStore,
                    *productContext);

        publisherWorker =
            std::make_unique<
                rim::PublisherWorker>(
                    publisherQueue,
                    *publishManager);

        routeProvider.Initialize(
            *productContext);

        routeExecutor.clear();

        for (const auto& [name, queues]
            : routeProvider.GetQueues())
        {
            routeExecutor.push_back(
                std::make_unique<
                    rim::RouteExecutor>(
                        *productContext,
                        queues,
                        domainStore,
                        *capabilityManager,
                        *facadeManager,
                        publisherQueue,
                        *capabilitySnapshotProvider,
                        *facadeSnapshotProvider));
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

    const auto domainId =
        g_context->productContext
            ->FindDomainId(
                {
                    rim::RIMIdType::Data,
                    static_cast<std::uint32_t>(
                        dataId)
                });

    const auto* storage =
        g_context->domainStore.Find(
            domainId);

    if (!storage)
    {
        return false;
    }

    return storage->Find(
        dataId,
        item);
}

} // namespace

extern "C"
{

const char*
RIM_GetProductName(void)
{
    return
        rim::ProductInfo::
            GetName().data();
}

RIStatus
RIM_GetVersionInfo(
    RIM_VERSION_INFO* versionInfo)
{
    if (versionInfo == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    const auto core =
        rim::CoreVersionInfo::Get();

    versionInfo->core.major =
        core.major;
    versionInfo->core.minor =
        core.minor;
    versionInfo->core.patch =
        core.patch;

    const auto product =
        rim::ProductVersionInfo::Get();

    versionInfo->product.major =
        product.major;
    versionInfo->product.minor =
        product.minor;
    versionInfo->product.patch =
        product.patch;

    return RI_SUCCESS;
}

#define RIM_VERSION_STRING \
    "Product/" RIM_PRODUCT_VERSION_STRING \
    " Core/" RIM_CORE_VERSION_STRING

const char*
RIM_GetVersionString(void)
{
    return RIM_VERSION_STRING;
}

int RIM_Create(void)
{
    if (g_context)
    {
        return RI_SUCCESS;
    }

    g_context =
        std::make_unique<
            RIManagerContext>();
	g_context->Initialize(rim::CreateProvider());

    return RI_SUCCESS;
}

int RIM_Destroy(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    g_context->publisherWorker->Stop();
    g_context->callbackWorker.Stop();

    // g_context->capabilityWorker.Stop();
    for (auto& route : g_context->routeExecutor)
    {
        route->Stop();
    }

    g_context->routeExecutor.clear();

    g_context.reset();

    return RI_SUCCESS;
}

int RIM_Start(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    for (auto& route : g_context->routeExecutor)
    {
        route->Start();
    }
    g_context->callbackWorker.Run();
    g_context->publisherWorker->Run();

    return RI_SUCCESS;
}

int RIM_Stop(void)
{
    if (!g_context)
    {
        return RI_NOT_INITIALIZED;
    }

    g_context->publisherWorker->Stop();
    g_context->callbackWorker.Stop();
    
    for (auto& route : g_context->routeExecutor)
    {
        route->Stop();
    }

    return RI_SUCCESS;
}

rim::CapabilityAccessor*
RIM_GetCapabilityAccessor()
{
    if (!g_context)
    {
        return nullptr;
    }

    return g_context->capabilityAccessor.get();
}

rim::FacadeAccessor*
RIM_GetFacadeAccessor()
{
    if (!g_context)
    {
        return nullptr;
    }

    return g_context->facadeAccessor.get();
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
            rim::RIMIdType>(
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

    // item.valueType =
    //     rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                value != 0);

    return g_context->dispatcher->Dispatch(item)
       ? RI_SUCCESS
       : RI_INVALID_PARAMETER;
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

    // item.valueType =
    //     rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::
            CreateInt32(
                value);

    return g_context->dispatcher->Dispatch(item)
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

    // item.valueType =
    //     rim::ValueType::kDouble;

    item.value =
        rim::RIMValueFactory::
            CreateDouble(
                value);

    return g_context->dispatcher->Dispatch(item)
       ? RI_SUCCESS
       : RI_INVALID_PARAMETER;
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

    const std::uint8_t* bytes{};
    std::size_t size{};

    if (!rim::RIMValueAccessor::GetBinary(
            item.value,
            bytes,
            size))
    {
        return RI_INTERNAL_ERROR;
    }

    if (bytes == nullptr)
    {
        return RI_NO_DATA;
    }

    binary->data =
        bytes;

    binary->size =
        static_cast<uint32_t>(
            size);

    return RI_SUCCESS;
}

RIStatus
RIM_SetBinary(
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

    rim::RIMDataItem item{};

    item.id =
        dataId;

    item.value =
        rim::RIMValueFactory::
            CreateBinary(
                static_cast<
                    const std::uint8_t*>(
                        data),
                size);

    return g_context->dispatcher->Dispatch(
               item)
           ? RI_SUCCESS
           : RI_INVALID_PARAMETER;
}

RIStatus RIM_GetCapabilityBool(RICapabilityId capabilityId, int* capability)
{
    if (capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* accessor = RIM_GetCapabilityAccessor();

    if (accessor == nullptr)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMValue value{};

    if (!accessor->TryGet(capabilityId, value))
    {
        return RI_NO_DATA;
    }

    bool found{};

    if (!rim::RIMValueAccessor::GetBool(value, found))
    {
        return RI_INTERNAL_ERROR;
    }

    *capability =
        found ? 1 : 0;

    return RI_SUCCESS;
}

RIStatus RIM_GetCapabilityInt32(RICapabilityId capabilityId, std::int32_t* capability)
{
    if (capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* accessor = RIM_GetCapabilityAccessor();

    if (accessor == nullptr)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMValue value{};

    if (!accessor->TryGet(capabilityId, value))
    {
        return RI_NO_DATA;
    }

    std::int32_t found{};

    if (!rim::RIMValueAccessor::GetInt32(value, found))
    {
        return RI_INTERNAL_ERROR;
    }

    *capability = found;

    return RI_SUCCESS;
}

RIStatus RIM_GetCapabilityDouble(RICapabilityId capabilityId, double* capability)
{
    if (capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* accessor = RIM_GetCapabilityAccessor();

    if (accessor == nullptr)
    {
        return RI_NOT_INITIALIZED;
    }

    rim::RIMValue value{};

    if (!accessor->TryGet(capabilityId, value))
    {
        return RI_NO_DATA;
    }

    double found{};

    if (!rim::RIMValueAccessor::GetDouble(value, found))
    {
        return RI_INTERNAL_ERROR;
    }

    *capability = found;

    return RI_SUCCESS;
}

}