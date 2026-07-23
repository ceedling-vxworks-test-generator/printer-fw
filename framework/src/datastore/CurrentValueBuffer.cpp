#include "datastore/CurrentValueBuffer.hpp"

namespace rim
{

bool CurrentValueBuffer::Post(const RIMDataItem& item)
{
    const int idx = ToIndex(item.id);
    if (idx < 0 || idx >= kRIMDataIdCount) return false;

    std::lock_guard<std::mutex> lk(mutex_);
    slots_[idx].data  = item;
    slots_[idx].dirty = true;
    return true;
}

std::vector<RIMDataItem> CurrentValueBuffer::TakeUpdatedValues()
{
    std::vector<RIMDataItem> out;

    std::lock_guard<std::mutex> lk(mutex_);
    for (auto& s : slots_) {
        if (s.dirty) {
            out.push_back(s.data);
            s.dirty = false;
        }
    }
    return out;
}

void CurrentValueBuffer::Clear()
{
    std::lock_guard<std::mutex> lk(mutex_);
    for (auto& s : slots_) s.dirty = false;
}

} // namespace rim
