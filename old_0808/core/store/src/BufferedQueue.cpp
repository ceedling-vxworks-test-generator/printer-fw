#include "BufferedQueue.hpp"

namespace rim
{

bool BufferedQueue::push(const RIMDataItem& item)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (shutdown_)
    {
        return false;
    }

    queue_.push(item);
    condition_.notify_one();

    return true;
}

bool BufferedQueue::Pop(RIMDataItem& item)
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

    item = queue_.front();
    queue_.pop();

    return true;
}

bool BufferedQueue::TryPop(RIMDataItem& item)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (queue_.empty())
    {
        return false;
    }

    item = queue_.front();
    queue_.pop();

    return true;
}

void BufferedQueue::shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);

    shutdown_ = true;
    condition_.notify_all();
}

} // namespace rim