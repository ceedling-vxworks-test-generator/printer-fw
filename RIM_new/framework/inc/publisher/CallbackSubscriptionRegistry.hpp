#pragma once

#include <vector>

#include "publisher/CallbackSubscriber.hpp"

namespace rim
{

class CallbackSubscriptionRegistry
{
public:

    void SubscribeEnvironment(
        EnvironmentCallback callback)
    {
        callbacks_.push_back(
            std::move(
                callback));
    }

    void NotifyEnvironment(
        const EnvironmentCapability&
            capability)
    {
        for (const auto& callback
             : callbacks_)
        {
            callback(
                capability);
        }
    }

private:

    std::vector<
        EnvironmentCallback>
        callbacks_;
};

}