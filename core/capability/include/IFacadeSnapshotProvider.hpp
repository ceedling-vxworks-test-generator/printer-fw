#pragma once

#include "RIId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class IFacadeSnapshotProvider
{
public:

    virtual ~IFacadeSnapshotProvider() = default;

    virtual RIMSnapshot Create(
        RIFacadeId facadeId) const = 0;
};

} // namespace rim