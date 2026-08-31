#pragma once

#include "EventQueue.hpp"
#include "QueuePolicy.hpp"

#include "PublisherInput.hpp"
#include "ProductContext.hpp"

namespace rim
{

class PublisherInputQueue
    : public EventQueue<
        PublisherInput,
        PriorityCompressionPolicy>
{
public:

    PublisherInputQueue()
        : EventQueue<
            PublisherInput,
            PriorityCompressionPolicy>()
    {
    }
};

} // namespace rim