#pragma once

#include "capability/ErrorCapability.hpp"
#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class ErrorRule
{
public:

    ErrorCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        ErrorCapability cap{};

        cap.upperDoorOpen =
            snapshot.upperDoorOpen;

        cap.rightDoorOpen =
            snapshot.rightDoorOpen;

        cap.leftDoorOpen =
            snapshot.leftDoorOpen;

        return cap;
    }
};

} // namespace rim
