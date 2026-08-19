#pragma once

#include <memory>

#include "EventQueue.hpp"
#include "QueuePolicy.hpp"
#include "RIMDataItem.hpp"
#include "IQueue.hpp"

namespace rim
{

// template<typename T, typename TPolicy>
// class TypedQueue: public IQueue<T>
// {
// public:

//     bool Push(const T& item) override
//     {
//         return queue_.Push(item);
//     }

//     bool TryPop(T& item) override
//     {
//         return queue_.TryPop(item);
//     }

//     bool WaitAndPop(T& item) override
//     {
//         return queue_.WaitAndPop(item);
//     }

//     void Shutdown() override
//     {
//         queue_.Shutdown();
//     }

// private:

//     EventQueue<T,TPolicy> queue_;
// };

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