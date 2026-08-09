#pragma once

#include "DeviceEvent.hpp"
#include "StoreInputQueue.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class AdapterDispatcher
{
public:

    AdapterDispatcher(
        const ProductDefinition& product,
        StoreInputQueue& queue)
        : product_(product),
          queue_(queue)
    {
    }

    bool Dispatch(
        const DeviceEvent& event);

private:

    const ProductDefinition& product_;

    StoreInputQueue& queue_;
};

} // namespace rim