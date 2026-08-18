#pragma once

#include <any>

#include "RIId.hpp"
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
    bool TryGet(
        RICapabilityId capabilityId,
        T& value) const
    {
        const auto* anyValue =
            store_.Find(
                capabilityId);

        if (anyValue == nullptr)
        {
            return false;
        }

        const auto* typedValue =
            std::any_cast<T>(
                anyValue);

        if (typedValue == nullptr)
        {
            return false;
        }

        value = *typedValue;

        return true;
    }

private:

    const CapabilityStore& store_;
};

} // namespace rim