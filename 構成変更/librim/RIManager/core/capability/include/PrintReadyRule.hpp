#pragma once

#include "PrintReadyCapability.hpp"
#include "ErrorRepository.hpp"
#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

class PrintReadyRule
{
public:

    PrintReadyCapability Evaluate(
        const RIMSnapshot& snapshot,
        const ErrorRepository& repository) const
    {
        PrintReadyCapability capability{};

        bool upperDoorOpen{};
        bool rightDoorOpen{};
        bool leftDoorOpen{};

        snapshot.TryGetBool(
            RIMDataId::kUpperDoorOpen,
            upperDoorOpen);

        snapshot.TryGetBool(
            RIMDataId::kRightDoorOpen,
            rightDoorOpen);

        snapshot.TryGetBool(
            RIMDataId::kLeftDoorOpen,
            leftDoorOpen);

        const bool doorsClosed =
            !upperDoorOpen &&
            !rightDoorOpen &&
            !leftDoorOpen;

        const bool noError =
            repository.GetAll().empty();

        capability.ready =
            doorsClosed &&
            noError;

        return capability;
    }
};

} // namespace rim