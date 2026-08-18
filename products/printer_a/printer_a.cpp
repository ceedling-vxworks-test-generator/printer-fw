#include "rim_api.h"

#include <memory>
#include <unordered_map>
#include <cstring>

#include "CapabilityStore.hpp"

#include "CapabilityAccessor.hpp"

#include "StoreInputQueue.hpp"
#include "RIMSnapshotManager.hpp"
#include "DataStoreWorker.hpp"
#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"

#include "CapabilityManager.hpp"
#include "CapabilityWorker.hpp"
#include "CapabilityAccessor.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

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

#include "printer_a_types.h"

namespace
{

template<typename T>
RIStatus GetStruct(
    RIDataId dataId,
    T& value)
{
    RI_BINARY binary{};

    const auto result =
        RIM_GetBinary(
            dataId,
            &binary);

    if (result != RI_SUCCESS)
    {
        return result;
    }

    if (binary.size != sizeof(T))
    {
        return RI_INTERNAL_ERROR;
    }

    std::memcpy(
        &value,
        binary.data,
        sizeof(T));

    return RI_SUCCESS;
}

template<typename T>
RIStatus SetStruct(
    RIDataId dataId,
    const T& value)
{
    return static_cast<RIStatus>(
        RIM_SetBinary(
            dataId,
            &value,
            sizeof(T)));
}

} // namespace

extern "C"
{

    
RIStatus
PrinterA_GetCapability(
    RICapabilityId capabilityId,
    void* capability)
{
    if (capability == nullptr)
    {
        return RI_INVALID_PARAMETER;
    }

    auto* accessor =
        RIM_GetCapabilityAccessor();

    if (accessor == nullptr)
    {
        return RI_NOT_INITIALIZED;
    }

    switch (capabilityId)
    {
    case RI_CAPABILITY_ENVIRONMENT:
    {
        auto* out =
            static_cast<
                rim::EnvironmentCapability*>(
                    capability);

        if (!accessor->TryGet(
                capabilityId,
                *out))
        {
            return RI_NO_DATA;
        }

        return RI_SUCCESS;
    }

    case RI_CAPABILITY_PRINT_READY:
    {
        auto* out =
            static_cast<
                rim::PrintReadyCapability*>(
                    capability);

        if (!accessor->TryGet(
                capabilityId,
                *out))
        {
            return RI_NO_DATA;
        }

        return RI_SUCCESS;
    }

    default:
        return RI_INVALID_PARAMETER;
    }
}

// int RIM_TestInjectTemperature(
//     double temperature)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_TEMPERATURE_SENSOR_A;

//     item.valueType =
//         rim::ValueType::kDouble;

//     item.value =
//         rim::RIMValueFactory::
//             CreateDouble(
//                 temperature);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectUpperDoorOpen(
//     int opened)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_UPPER_DOOR_OPEN;

//     item.valueType =
//         rim::ValueType::kBool;

//     item.value =
//         rim::RIMValueFactory::
//             CreateBool(
//                 opened != 0);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectRightDoorOpen(
//     int opened)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_RIGHT_DOOR_OPEN;

//     item.valueType =
//         rim::ValueType::kBool;

//     item.value =
//         rim::RIMValueFactory::
//             CreateBool(
//                 opened != 0);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectLeftDoorOpen(
//     int opened)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_LEFT_DOOR_OPEN;

//     item.valueType =
//         rim::ValueType::kBool;

//     item.value =
//         rim::RIMValueFactory::
//             CreateBool(
//                 opened != 0);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectStapleLevel(
//     std::int32_t level)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_STAPLE_LEVEL;

//     item.valueType =
//         rim::ValueType::kInt32;

//     item.value =
//         rim::RIMValueFactory::
//             CreateInt32(
//                 level);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectJobActive(
//     int active)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_JOB_ACTIVE;

//     item.value =
//         rim::RIMValueFactory::CreateBool(
//             active != 0);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }

// int RIM_TestInjectJobId(
//     int jobId)
// {
//     if (!g_context)
//     {
//         return RI_NOT_INITIALIZED;
//     }

//     rim::RIMDataItem item{};

//     item.id =
//         RI_DATA_JOB_ID;

//     item.value =
//         rim::RIMValueFactory::CreateInt32(
//             jobId);

//     g_context->storeQueue.Push(
//         item);

//     return RI_SUCCESS;
// }


RIStatus
PrinterA_SetErrorList(
    const RI_FAULT_INFO_LIST* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_ERROR_LIST,
        *list);
}

RIStatus
PrinterA_GetErrorList(
    RI_FAULT_INFO_LIST* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_ERROR_LIST,
        *list);
}


}