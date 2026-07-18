#pragma once

#include "capability/PrintReadyCapability.hpp"
#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class PrintReadyRule
{
public:

    PrintReadyCapability Evaluate(
        const RIMSnapshot& snapshot) const
    {
        PrintReadyCapability cap{};

        cap.ready =
            !snapshot.upperDoorOpen &&
            !snapshot.rightDoorOpen &&
            !snapshot.leftDoorOpen;

        return cap;
    }
};

} // namespace rim