#pragma once

#include "ICapabilitySnapshotProvider.hpp"

#include "CapabilitySnapshotResolver.hpp"
#include "SnapshotAccessor.hpp"

namespace rim
{

class CapabilitySnapshotProvider
    : public ICapabilitySnapshotProvider
{
public:

    CapabilitySnapshotProvider(
        const CapabilitySnapshotResolver& resolver,
        const SnapshotAccessor& accessor)
        :
        resolver_(resolver),
        accessor_(accessor)
    {
    }

    RIMSnapshot Create(
        RICapabilityId changedDataId) const override
    {
        const auto domains =
            resolver_.ResolveSnapshotDomains(
                changedDataId);

        return accessor_.CreateSnapshot(
            domains);
    }

private:

    const CapabilitySnapshotResolver&
        resolver_;

    const SnapshotAccessor&
        accessor_;
};

} // namespace rim