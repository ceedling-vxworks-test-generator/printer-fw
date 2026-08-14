#pragma once

#include <string_view>
#include <vector>

#include "CapabilityAccessor.hpp"
#include "DataAccessor.hpp"
#include "SnapshotAccessor.hpp"
#include "rim_data_id.h"

namespace rim
{

class AccessorFacade
{
public:

    AccessorFacade(
        DataAccessor& dataAccessor,
        CapabilityAccessor& capabilityAccessor,
        SnapshotAccessor& snapshotAccessor)
        :
        dataAccessor_(dataAccessor),
        capabilityAccessor_(capabilityAccessor),
        snapshotAccessor_(snapshotAccessor)
    {
    }

    DataAccessor&
    Data()
    {
        return dataAccessor_;
    }

    const DataAccessor&
    Data() const
    {
        return dataAccessor_;
    }

    CapabilityAccessor&
    Capability()
    {
        return capabilityAccessor_;
    }

    const CapabilityAccessor&
    Capability() const
    {
        return capabilityAccessor_;
    }

    SnapshotAccessor&
    Snapshot()
    {
        return snapshotAccessor_;
    }

    const SnapshotAccessor&
    Snapshot() const
    {
        return snapshotAccessor_;
    }

    AccessId
    CreateSnapshotByDomains(
        const std::vector<std::string_view>& domains);

    AccessId
    CreateSnapshotByDataId(
        RIDataId id);

    AccessId
    CreateSnapshotByDataIds(
        const std::vector<RIDataId>& ids);

    // AccessId
    // CreateSnapshotByCapability(
    //     std::string_view capability);

    // AccessId
    // CreateSnapshotByCapabilities(
    //     const std::vector<std::string_view>& capabilities);        

private:

    DataAccessor&
        dataAccessor_;

    CapabilityAccessor&
        capabilityAccessor_;

    SnapshotAccessor&
        snapshotAccessor_;
};

} // namespace rim