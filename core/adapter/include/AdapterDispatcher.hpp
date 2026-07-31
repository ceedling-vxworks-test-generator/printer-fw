#pragma once

#include "DeviceEvent.hpp"
#include "StoreInputQueue.hpp"

namespace rim
{

class AdapterDispatcher
{
public:

    explicit AdapterDispatcher(
        StoreInputQueue& queue)
        : queue_(queue)
    {
    }

    bool Dispatch(
        const DeviceEvent& event);

private:

    StoreInputQueue& queue_;
};

} // namespace rim