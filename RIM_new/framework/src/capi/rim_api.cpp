#include "rim_api.h"

#include "accessor/RIManager.hpp"

#include "capability/EnvironmentCapability.hpp"
#include "capability/ErrorCapability.hpp"
#include "capability/PrintReadyCapability.hpp"

namespace
{

struct RIManagerContext
{
    rim::SubscriberMailbox mailbox;

    rim::NotificationReceiver manager;

    rim::RIManager facade;

    RIManagerContext()
        : manager(mailbox)
        , facade(manager)
    {
    }
};

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

    delete static_cast<
        RIManagerContext*>(
            handle);

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

}