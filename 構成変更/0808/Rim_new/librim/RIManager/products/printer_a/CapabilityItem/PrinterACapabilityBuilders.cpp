#include "PrinterACapabilityBuilders.hpp"

#include "EnvironmentCapability.hpp"
#include "PrintReadyCapability.hpp"

#include "RIMSnapshot.hpp"

namespace rim
{

std::any
BuildEnvironmentCapability(
    const RIMSnapshot& snapshot)
{
    EnvironmentCapability capability{};

    snapshot.TryGetDouble(
        RI_DATA_TEMPERATURE_SENSOR_A,
        capability.temperature);

    snapshot.TryGetDouble(
        RI_DATA_HUMIDITY_SENSOR,
        capability.humidity);

    return capability;
}

std::any
BuildPrintReadyCapability(
    const RIMSnapshot& snapshot)
{
    PrintReadyCapability capability{};

    bool upperDoorOpen{};
    bool rightDoorOpen{};
    bool leftDoorOpen{};

    snapshot.TryGetBool(
        RI_DATA_UPPER_DOOR_OPEN,
        upperDoorOpen);

    snapshot.TryGetBool(
        RI_DATA_RIGHT_DOOR_OPEN,
        rightDoorOpen);

    snapshot.TryGetBool(
        RI_DATA_LEFT_DOOR_OPEN,
        leftDoorOpen);

    capability.ready =
        !upperDoorOpen &&
        !rightDoorOpen &&
        !leftDoorOpen;

    return capability;
}

// std::any
// BuildConsumableCapability(
//     const RIMSnapshot& snapshot)
// {
//     ConsumableCapability capability{};

//     snapshot.TryGetInt32(
//         RI_DATA_STAPLE_LEVEL,
//         capability.stapleLevel);

//     snapshot.TryGetInt32(
//         RI_DATA_TONER_LEVEL,
//         capability.tonerLevel);

//     return capability;
// }

// std::any
// BuildJobCapability(
//     const RIMSnapshot& snapshot)
// {
//     JobCapability capability{};

//     snapshot.TryGetBool(
//         RI_DATA_JOB_ACTIVE,
//         capability.jobActive);

//     snapshot.TryGetInt32(
//         RI_DATA_JOB_ID,
//         capability.jobId);

//     return capability;
// }

} // namespace rim