#pragma once

#include <string_view>
#include <vector>

#include "CapabilityAccessor.hpp"
#include "DataAccessor.hpp"
#include "SnapshotAccessor.hpp"

namespace rim
{

class AccessorFacade
{
public:

    AccessorFacade(
        DataAccessor& dataAccessor,
        CapabilityAccessor& capabilityAccessor)
        :
        dataAccessor_(dataAccessor),
        capabilityAccessor_(capabilityAccessor)
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

private:

    DataAccessor&
        dataAccessor_;

    CapabilityAccessor&
        capabilityAccessor_;
};

} // namespace rim