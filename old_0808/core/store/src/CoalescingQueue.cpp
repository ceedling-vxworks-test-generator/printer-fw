#include "CoalescingQueue.hpp"

namespace rim
{

bool CoalescingQueue::push(const RIMDataItem& item)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (shutdown_)
    {
        return false;
    }

    queue_[item.id] = item;
    condition_.notify_one();

    return true;
}

bool CoalescingQueue::Pop(RIMDataItem& item)
{
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(
        lock,
        [this]
        {
            return shutdown_ || !queue_.empty();
        });

    if (shutdown_ && queue_.empty())
    {
        return false;
    }

    auto it = queue_.begin();

    item = it->second;
    queue_.erase(it);

    return true;
}

bool CoalescingQueue::TryPop(RIMDataItem& item)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty())
    {
        return false;
    }

    auto it = queue_.begin();

    item = it->second;
    queue_.erase(it);

    return true;
}


void CoalescingQueue::shutdown()
{
    std::unique_lock<std::mutex> lock(mutex_);

    shutdown_ = true;
    condition_.notify_all();
}

} // namespace rim