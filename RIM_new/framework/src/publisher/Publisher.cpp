#include "publisher/Publisher.hpp"

namespace rim
{

void Publisher::Init()     { count_ = 0; }
void Publisher::Shutdown() { count_ = 0; }

RIMResult Publisher::Subscribe(ISubscriber* subscriber)
{
    if (subscriber == nullptr) return RIMResult::kErrInvalidArg;
    if (count_ >= kMaxSubscribers) return RIMResult::kErrFull;
    subscribers_[count_] = subscriber;
    ++count_;
    return RIMResult::kOk;
}

void Publisher::Publish(const CapabilitySet& cap)
{
    for (std::size_t i = 0; i < count_; ++i)
        subscribers_[i]->OnPublish(cap);
}

} // namespace rim
