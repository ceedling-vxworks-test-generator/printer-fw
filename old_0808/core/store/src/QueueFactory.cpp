#include "QueueFactory.hpp"

#include "BufferedQueue.hpp"
#include "CoalescingQueue.hpp"

namespace rim
{

std::unique_ptr<DataStoreQueue> QueueFactory::Create(QueuePolicy policy)
{
    switch (policy)
    {
    case QueuePolicy::Buffered:

        return std::make_unique<BufferedQueue>();

    case QueuePolicy::Coalescing:

        return std::make_unique<CoalescingQueue>();
    }

    return nullptr;
}

}