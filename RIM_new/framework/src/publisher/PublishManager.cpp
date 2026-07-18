#include "publisher/PublishManager.hpp"

namespace rim
{

bool PublishManager::HasEnvironmentChanged()
{
    const auto& current =
        store_.GetEnvironment();

    const bool changed =
        CapabilityDiffChecker::HasChanged(
            previousEnvironment_,
            current);

    if (changed)
    {
        notifyManager_.Notify(
            current);
    }

    previousEnvironment_ =
        current;

    return changed;
}

bool PublishManager::HasErrorChanged()
{
    const auto& current =
        store_.GetError();

    const bool changed =
        CapabilityDiffChecker::HasChanged(
            previousError_,
            current);

    if (changed)
    {
        notifyManager_.Notify(
            current);
    }

    previousError_ = current;

    return changed;
}

bool PublishManager::HasPrintReadyChanged()
{
    const auto& current =
        store_.GetPrintReady();

    const bool changed =
        CapabilityDiffChecker::HasChanged(
            previousPrintReady_,
            current);

    if (changed)
    {
        notifyManager_.Notify(
            current);
    }

    previousPrintReady_ = current;

    return changed;
}

} // namespace rim