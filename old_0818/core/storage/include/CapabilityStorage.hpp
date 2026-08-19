#pragma once

#include <any>
#include <unordered_map>

#include "printer_a.h"

namespace rim
{

class CapabilityStorage
{
public:

    void Store(
        RICapabilityId capabilityId,
        std::any value)
    {
        values_[capabilityId] =
            std::move(value);
    }

    const std::any*
    Find(
        RICapabilityId capabilityId) const
    {
        const auto it =
            values_.find(
                capabilityId);

        if (it == values_.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    bool Exists(
        RICapabilityId capabilityId) const
    {
        return
            values_.find(
                capabilityId)
            != values_.end();
    }

    std::size_t Size() const
    {
        return values_.size();
    }

    std::size_t BucketCount() const
    {
        return values_.bucket_count();
    }

    float LoadFactor() const
    {
        return values_.load_factor();
    }

    float MaxLoadFactor() const
    {
        return values_.max_load_factor();
    }

private:

    std::unordered_map<
        RICapabilityId,
        std::any>
        values_;
};

} // namespace rim