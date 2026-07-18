#pragma once

#include "capability/MachineCapabilityStore.hpp"

#include "publisher/CapabilityDiffChecker.hpp"
#include "publisher/ChangeNotifyManager.hpp"

namespace rim
{

class PublishManager
{
public:

    PublishManager(
        const MachineCapabilityStore& store,
        ChangeNotifyManager& notifyManager)
        : store_(store)
        , notifyManager_(notifyManager)
    {
    }

    bool HasEnvironmentChanged();

    bool HasErrorChanged();

    bool HasPrintReadyChanged();

private:

    const MachineCapabilityStore& store_;

    ChangeNotifyManager& notifyManager_;

    EnvironmentCapability previousEnvironment_{};

    ErrorCapability previousError_{};

    PrintReadyCapability previousPrintReady_{};
};

} // namespace rim