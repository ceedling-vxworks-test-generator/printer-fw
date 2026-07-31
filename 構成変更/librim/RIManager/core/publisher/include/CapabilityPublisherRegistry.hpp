#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ICapabilityPublisher.hpp"

namespace rim
{

class CapabilityPublisherRegistry
{
public:

    void Register(
        const std::string& capabilityId,
        std::unique_ptr<ICapabilityPublisher>
            publisher);

    ICapabilityPublisher* Find(
        const std::string& capabilityId);

private:

    std::unordered_map<
        std::string,
        std::unique_ptr<ICapabilityPublisher>>
            publishers_;
};

} // namespace rim