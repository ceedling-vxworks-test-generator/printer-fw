#pragma once

#include "datastore/RIMDataItem.hpp"

#include <mutex>
#include <unordered_map>

namespace rim
{

class ValueStore
{
public:

    void Store(
        const RIMDataItem& item)
    {
        std::lock_guard<std::mutex> lock(m_);

        d_[item.id] = item;
    }

    bool Find(
        RIMDataId id,
        RIMDataItem& out) const
    {
        std::lock_guard<std::mutex> lock(m_);

        const auto it = d_.find(id);

        if (it == d_.end())
        {
            return false;
        }

        out = it->second;

        return true;
    }

private:

    mutable std::mutex m_;

    std::unordered_map<
        RIMDataId,
        RIMDataItem> d_;
};

} // namespace rim