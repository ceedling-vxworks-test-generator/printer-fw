#pragma once

#include "RIMId.hpp"

namespace rim
{

struct CapabilityInput
{
    RIMId changedId
    {
        RIMIdType::Data,
        kInvalidRIMObjectId
    };

    RIMId CompressionKey() const
    {
        return changedId;
    }
};

} // namespace rim