#pragma once

#include "RIMDataItem.hpp"

#include <mutex>
#include <unordered_map>
#include <vector>

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

    std::vector<RIMDataItem> GetAll() const
    {
        std::lock_guard<std::mutex> lock(m_);

        std::vector<RIMDataItem> result;

        result.reserve(
            d_.size());

        for (const auto& [id, item] : d_)
        {
            result.push_back(
                item);
        }

        return result;
    }

private:

    mutable std::mutex m_;

    std::unordered_map<
        RIMDataId,
        RIMDataItem> d_;
};

} // namespace rim