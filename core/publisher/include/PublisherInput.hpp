#pragma once

#include "EventPriority.hpp"
#include "CompressionPolicy.hpp"
#include "NotificationTarget.hpp"

namespace rim
{
    struct PublisherInput
    {
        NotificationTarget target;
        EventPriority priority;
        CompressionPolicy compressionPolicy;
    };
} // namespace rim