#include "CapabilityPublisherRegistry.hpp"

namespace rim
{

void CapabilityPublisherRegistry::Register(
    const std::string& capabilityId,
    std::unique_ptr<ICapabilityPublisher>
        publisher)
{
    publishers_[capabilityId] =
        std::move(publisher);
}

ICapabilityPublisher*
CapabilityPublisherRegistry::Find(
    const std::string& capabilityId)
{
    const auto it =
        publishers_.find(
            capabilityId);

    if (it == publishers_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

} // namespace rim