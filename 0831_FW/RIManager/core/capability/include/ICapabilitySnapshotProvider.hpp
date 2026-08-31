#pragma once

#include "RIId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class ICapabilitySnapshotProvider
{
public:

    virtual ~ICapabilitySnapshotProvider() = default;

    virtual RIMSnapshot Create(
        RICapabilityId capabilityId) const = 0;
};

} // namespace rim