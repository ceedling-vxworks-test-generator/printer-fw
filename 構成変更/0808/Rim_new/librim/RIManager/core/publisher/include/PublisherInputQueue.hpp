#pragma once

#include "EventQueue.hpp"
#include "QueuePolicy.hpp"

#include "PublisherInput.hpp"

namespace rim
{

    using PublisherInputQueue =
        EventQueue<
            PublisherInput,
            PriorityCompressionPolicy>;

} // namespace rim