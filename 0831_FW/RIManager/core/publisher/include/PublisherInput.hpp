#pragma once

#include "EventPriority.hpp"
#include "CompressionPolicy.hpp"
#include "RIMId.hpp"

namespace rim
{
    struct PublisherInput
    {
        RIMId target;
        EventPriority priority;
        CompressionPolicy compressionPolicy;
    };
} // namespace rim