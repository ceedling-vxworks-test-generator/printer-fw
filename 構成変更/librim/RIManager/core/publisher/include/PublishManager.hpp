#pragma once

#include <string>

#include "CapabilityPublisherRegistry.hpp"

namespace rim
{

class PublishManager
{
public:

    explicit PublishManager(
        CapabilityPublisherRegistry& registry)
        : registry_(registry)
    {
    }

    void Publish(
        const std::string& capabilityId);

private:

    CapabilityPublisherRegistry&
        registry_;
};

}