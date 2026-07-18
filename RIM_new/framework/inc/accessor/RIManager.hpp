#pragma once

#include "accessor/NotificationReceiver.hpp"

namespace rim
{

class RIManager
{
public:

    explicit RIManager(
        NotificationReceiver& manager)
        : manager_(manager)
    {
    }

    bool TryGetEnvironment(
        EnvironmentCapability& capability)
    {
        return manager_.TryGetEnvironment(
            capability);
    }

    bool TryGetError(
        ErrorCapability& capability)
    {
        return manager_.TryGetError(
            capability);
    }

    bool TryGetPrintReady(
        PrintReadyCapability& capability)
    {
        return manager_.TryGetPrintReady(
            capability);
    }

private:

    NotificationReceiver& manager_;
};

} // namespace rim
