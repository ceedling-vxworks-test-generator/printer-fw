#pragma once

#include <any>
#include <new>
#include <stdexcept>

#include "CapabilityStore.hpp"

namespace rim
{

class CapabilityAccessor
{
public:

    explicit CapabilityAccessor(
        const CapabilityStore& store)
        : store_(store)
    {
    }

    template<typename T>
    T Get(
        RICapabilityId capabilityId) const
    {
        const auto* value =
            store_.Find(
                capabilityId);

        if (value == nullptr)
        {
            throw std::runtime_error(
                "Capability not found");
        }

        return std::any_cast<T>(
            *value);
    }

private:

    const CapabilityStore& store_;
};

} // namespace rim