#pragma once

#include <any>
#include <new>
#include <unordered_map>

#include "RIId.hpp"

namespace rim
{

class CapabilityStore
{
public:

    void Store(
        RICapabilityId id,
        std::any value)
    {
        values_[id] =
            std::move(
                value);
    }

    const std::any*
    Find(
        RICapabilityId id) const
    {
        const auto it =
            values_.find(
                id);

        if (it == values_.end())
        {
            return nullptr;
        }

        return &it->second;
    }

private:

    std::unordered_map<
        RICapabilityId,
        std::any>
            values_;
};

} // namespace rim