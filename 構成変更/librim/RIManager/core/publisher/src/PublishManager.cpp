#include "PublishManager.hpp"

namespace rim
{

void PublishManager::Publish(
    const std::string& capabilityId)
{
    auto* publisher =
        registry_.Find(
            capabilityId);

    if (publisher == nullptr)
    {
        return;
    }

    publisher->Publish();
}

} // namespace rim