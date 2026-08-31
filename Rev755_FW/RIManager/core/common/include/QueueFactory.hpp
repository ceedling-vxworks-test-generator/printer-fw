#pragma once

#include <memory>

#include "EventQueue.hpp"
#include "QueuePolicy.hpp"
#include "RIMDataItem.hpp"
#include "IQueue.hpp"

namespace rim
{

class QueueFactory
{
public:

    template<typename T>
    static std::unique_ptr<IQueue<T>>
    Create(QueuePolicy policy)
    {
        switch (policy)
        {
        case QueuePolicy::Buffered:

            return std::make_unique<EventQueue<T, FifoPolicy>>();

        case QueuePolicy::Coalescing:

            return std::make_unique<EventQueue<T, CoalescingPolicy>>();

        // case QueuePolicy::Priority:

        //     return std::make_unique<EventQueue<T, PriorityPolicy>>();

        // case QueuePolicy::PriorityCompression:

        //     return std::make_unique<EventQueue<T, PriorityCompressionPolicy>>();
        }

        return {};
    }

};

}