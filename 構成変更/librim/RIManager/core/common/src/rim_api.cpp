#include "rim_api.h"

#include <memory>
#include <unordered_map>

#include "RIManager.hpp"
#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "ValueStore.hpp"
#include "AggregateRIMSnapshotReader.hpp"
#include "DataStoreWorker.hpp"
#include "RIMValueFactory.hpp"

#include "EnvironmentCapability.hpp"
#include "ErrorCapability.hpp"
#include "PrintReadyCapability.hpp"
#include "CapabilityInputQueue.hpp"
#include "CapabilityManager.hpp"
#include "CapabilityWorker.hpp"
#include "MachineCapabilityStore.hpp"

#include "PublisherInputQueue.hpp"
#include "PublisherWorker.hpp"
#include "PublishManager.hpp"
#include "CallbackSubscriptionRegistry.hpp"
#include "ChangeNotifyManager.hpp"
#include "GenericCapabilityPublisher.hpp"

#include "IErrorDefinitionRegistry.hpp"

namespace
{

struct RIManagerContext
{
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
    // Error
    //
    rim::ErrorRepository
        errorRepository;

    //
    // Datastore
    //

    rim::ValueStore
        valueStore;

    rim::AggregateRIMSnapshotReader
        snapshotReader;

    //
    // Capability
    //

    rim::MachineCapabilityStore
        capabilityStore;

    rim::CapabilityManager
        capabilityManager;

    //
    // Publisher
    //

    rim::SubscriberMailbox
        mailbox;

    rim::CallbackSubscriptionRegistry
        callbackRegistry;

    rim::ChangeNotifyManager
        notifyManager;

    rim::PublishManager
        publishManager;

    rim::CapabilityPublisherRegistry
        publisherRegistry;

    //
    // Worker
    //

    rim::DataStoreWorker
        dataStoreWorker;

    rim::CapabilityWorker
        capabilityWorker;

    rim::PublisherWorker
        publisherWorker;

    std::unordered_map<
        std::uint64_t,
        std::uint64_t>
        environmentSubscriptions;

    //
    // Accessor
    //

    rim::NotificationReceiver
        receiver;

    rim::RIManager
        facade;

    rim::CapabilityAccessor
        capabilityAccessor;

    //
    // Error
    //
    const rim::IErrorDefinitionRegistry*
        errorRegistry {};

    RIManagerContext()
        : snapshotReader(
            valueStore)
        , capabilityManager(
            capabilityStore,
            errorRepository)
        , notifyManager(
            mailbox,
            callbackRegistry)
        , publisherRegistry()
        , publishManager(
            publisherRegistry)
        , errorRegistry(
            nullptr)
        , dataStoreWorker(
            storeQueue,
            valueStore,
            snapshotReader,
            capabilityQueue)
        , capabilityWorker(
            capabilityQueue,
            capabilityManager,
            capabilityStore,
            publisherQueue)
        , publisherWorker(
            publisherQueue,
            publishManager)
        , capabilityAccessor(
            capabilityStore)
        , receiver(
            mailbox)
        , facade(
            receiver)
    {
        publisherRegistry.Register(
            "Environment",
            std::make_unique<
                rim::GenericCapabilityPublisher>(
                [this]
                {
                    notifyManager.Notify(
                        capabilityStore.GetEnvironment());
                }));

        publisherRegistry.Register(
            "Error",
            std::make_unique<
                rim::GenericCapabilityPublisher>(
                [this]
                {
                    notifyManager.Notify(
                        capabilityStore.GetError());
                }));

        publisherRegistry.Register(
            "PrintReady",
            std::make_unique<
                rim::GenericCapabilityPublisher>(
                [this]
                {
                    notifyManager.Notify(
                        capabilityStore.GetPrintReady());
                }));

        publisherRegistry.Register(
            "Consumable",
            std::make_unique<
                rim::GenericCapabilityPublisher>(
                [this]
                {
                    notifyManager.Notify(
                        capabilityStore.GetConsumable());
                }));

        publisherRegistry.Register(
            "Job",
            std::make_unique<
                rim::GenericCapabilityPublisher>(
                [this]
                {
                    notifyManager.Notify(
                        capabilityStore.GetJob());
                }));
    }

};

void ReevaluateCapabilities(
    RIManagerContext& context)
{
    const auto snapshot =
        context.snapshotReader.Read();

    context.capabilityQueue.Push(
        snapshot);
}

rim::ErrorSeverity
ResolveErrorSeverity(
    const RIManagerContext& context,
    std::uint32_t errorCode)
{
    rim::ErrorSeverity severity =
        rim::ErrorSeverity::kError;

    if (context.errorRegistry != nullptr)
    {
        context.errorRegistry
            ->TryGetSeverity(
                errorCode,
                severity);
    }

    return severity;
}


}


extern "C"
{


int RIManager_Create(
    RIM_HANDLE* handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    *handle =
        new RIManagerContext();

    return RI_SUCCESS;
}

int RIManager_Destroy(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    context->publisherWorker.Stop();

    context->capabilityWorker.Stop();

    context->dataStoreWorker.Stop();

    delete context;

    return RI_SUCCESS;
}

int RIManager_Start(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    context->dataStoreWorker.Run();

    context->capabilityWorker.Run();

    context->publisherWorker.Run();

    return RI_SUCCESS;
}

int RIManager_Stop(
    RIM_HANDLE handle)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    context->publisherWorker.Stop();

    context->capabilityWorker.Stop();

    context->dataStoreWorker.Stop();

    return RI_SUCCESS;
}

int RIManager_GetEnvironment(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    return context->facade.
               TryGetEnvironment(
                   *static_cast<
                       rim::EnvironmentCapability*>(
                           capability))
           ? RI_SUCCESS
           : RI_NO_DATA;
}

int RIManager_GetCurrentEnvironment(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    *static_cast<
        rim::EnvironmentCapability*>(
            capability)
        =
        context->capabilityAccessor.
            GetEnvironment();

    return RI_SUCCESS;
}

int RIManager_AddError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    if (context->errorRepository.Add(
        {
            errorCode,
            rim::ErrorState::kActive,
            ResolveErrorSeverity(
                *context,
                errorCode)
        }))
    {
        ReevaluateCapabilities(
            *context);
    }

    return RI_SUCCESS;
}

int RIManager_RemoveError(
    RIM_HANDLE handle,
    uint32_t errorCode)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    context->errorRepository.Remove(
        errorCode);

    ReevaluateCapabilities(
        *context);

    return RI_SUCCESS;
}

int RIManager_SetErrorState(
    RIM_HANDLE handle,
    uint32_t errorCode,
    int state)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    context->errorRepository.SetState(
        errorCode,
        static_cast<rim::ErrorState>(
            state));

    ReevaluateCapabilities(
        *context);

    return RI_SUCCESS;
}





int RIManager_GetError(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    return context->facade.
               TryGetError(
                   *static_cast<
                       rim::ErrorCapability*>(
                           capability))
           ? RI_SUCCESS
           : RI_NO_DATA;
}

int RIManager_GetCurrentError(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    *static_cast<
        rim::ErrorCapability*>(
            capability)
        =
        context->capabilityAccessor.
            GetError();

    return RI_SUCCESS;
}

int RIManager_GetPrintReady(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    return context->facade.
               TryGetPrintReady(
                   *static_cast<
                       rim::PrintReadyCapability*>(
                           capability))
           ? RI_SUCCESS
           : RI_NO_DATA;
}

int RIManager_GetCurrentPrintReady(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    *static_cast<
        rim::PrintReadyCapability*>(
            capability)
        =
        context->capabilityAccessor.
            GetPrintReady();

    return RI_SUCCESS;
}

int RIManager_GetCurrentConsumable(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    *static_cast<
        rim::ConsumableCapability*>(
            capability)
        =
        context->capabilityAccessor.
            GetConsumable();

    return RI_SUCCESS;
}

int RIManager_GetCurrentJob(
    RIM_HANDLE handle,
    void* capability)
{
    if (handle == nullptr ||
        capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    *static_cast<rim::JobCapability*>(
        capability)
        =
        context->capabilityAccessor
            .GetJob();

    return RI_SUCCESS;
}



int RIManager_SubscribeEnvironment(
    RIM_HANDLE handle,
    RIEnvironmentCallback callback,
    void* userData,
    std::uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    const auto id =
        context->callbackRegistry.
            SubscribeEnvironment(
                [callback,
                 userData]
                (
                    rim::SubscriptionId id,
                    const rim::EnvironmentCapability&
                        capability)
                {
                    callback(
                        id,
                        &capability,
                        userData);
                });

    *subscriptionId = id;

    return RI_SUCCESS;
}

int RIManager_SubscribeError(
    RIM_HANDLE handle,
    RIErrorCallback callback,
    void* userData,
    uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    const auto id =
        context->callbackRegistry.
            SubscribeError(
                [callback,
                 userData]
                (
                    rim::SubscriptionId id,
                    const rim::ErrorCapability&
                        capability)
                {
                    callback(
                        id,
                        &capability,
                        userData);
                });

    *subscriptionId = id;

    return RI_SUCCESS;
}

int RIManager_SubscribePrintReady(
    RIM_HANDLE handle,
    RIPrintReadyCallback callback,
    void* userData,
    uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    const auto id =
        context->callbackRegistry.
            SubscribePrintReady(
                [callback,
                 userData]
                (
                    rim::SubscriptionId id,
                    const rim::PrintReadyCapability& capability)
                {
                    callback(
                        id,
                        &capability,
                        userData);
                });

    *subscriptionId = id;

    return RI_SUCCESS;
}

int RIManager_SubscribeConsumable(
    RIM_HANDLE handle,
    RIConsumableCallback callback,
    void* userData,
    uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    const auto id =
        context->callbackRegistry.
            SubscribeConsumable(
                [callback,
                 userData]
                (
                    rim::SubscriptionId id,
                    const rim::ConsumableCapability&
                        capability)
                {
                    callback(
                        id,
                        &capability,
                        userData);
                });

    *subscriptionId = id;

    return RI_SUCCESS;
}

int RIManager_SubscribeJob(
    RIM_HANDLE handle,
    RIJobCallback callback,
    void* userData,
    uint64_t* subscriptionId)
{
    if (handle == nullptr ||
        callback == nullptr ||
        subscriptionId == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    const auto id =
        context->callbackRegistry
            .SubscribeJob(
                [callback,
                 userData]
                (
                    rim::SubscriptionId id,
                    const rim::JobCapability& capability)
                {
                    callback(
                        id,
                        &capability,
                        userData);
                });

    *subscriptionId = id;

    return RI_SUCCESS;
}


int RIManager_Unsubscribe(
    RIM_HANDLE handle,
    std::uint64_t subscriptionId)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    return context->callbackRegistry.
               Unsubscribe(
                   subscriptionId)
           ? RI_SUCCESS
           : RI_NO_DATA;
}



int RIManager_TestInjectTemperature(
    RIM_HANDLE handle,
    double temperature)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kTemperatureSensorA;

    item.valueType =
        rim::ValueType::kDouble;

    item.value =
        rim::RIMValueFactory::
            CreateDouble(
                temperature);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectUpperDoorOpen(
    RIM_HANDLE handle,
    int opened)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kUpperDoorOpen;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectRightDoorOpen(
    RIM_HANDLE handle,
    int opened)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kRightDoorOpen;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectLeftDoorOpen(
    RIM_HANDLE handle,
    int opened)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kLeftDoorOpen;

    item.valueType =
        rim::ValueType::kBool;

    item.value =
        rim::RIMValueFactory::
            CreateBool(
                opened != 0);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectStapleLevel(
    RIM_HANDLE handle,
    std::int32_t level)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<
            RIManagerContext*>(
                handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kStapleLevel;

    item.valueType =
        rim::ValueType::kInt32;

    item.value =
        rim::RIMValueFactory::
            CreateInt32(
                level);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectJobActive(
    RIM_HANDLE handle,
    int active)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kJobActive;

    item.value =
        rim::RIMValueFactory::CreateBool(
            active != 0);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

int RIManager_TestInjectJobId(
    RIM_HANDLE handle,
    int jobId)
{
    if (handle == nullptr)
    {
        return RI_INVALID_HANDLE;
    }

    auto* context =
        static_cast<RIManagerContext*>(
            handle);

    rim::RIMDataItem item{};

    item.id =
        rim::RIMDataId::kJobId;

    item.value =
        rim::RIMValueFactory::CreateInt32(
            jobId);

    context->storeQueue.Push(
        item);

    return RI_SUCCESS;
}

// void RebuildCapabilities(
//     RIManagerContext& context)
// {
//     const auto snapshot =
//         context.snapshotReader.Read();

//     context.capabilityManager.
//         Evaluate(
//             snapshot);

//     context.publishManager.
//         HasErrorChanged();

//     context.publishManager.
//         HasPrintReadyChanged();
// }




}