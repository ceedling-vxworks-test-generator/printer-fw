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
        RIDataId changedDataId) const = 0;
};

} // namespace rim