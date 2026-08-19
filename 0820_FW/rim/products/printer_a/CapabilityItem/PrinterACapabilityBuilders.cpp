#include "PrinterACapabilityBuilders.hpp"

#include "MachineCapabilityStore.hpp"

#include "ConsumableCapability.hpp"
#include "EnvironmentCapability.hpp"
#include "JobCapability.hpp"
#include "PrintReadyCapability.hpp"

#include "RIMDataId.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

void BuildEnvironmentCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store)
{
    EnvironmentCapability capability{};

    snapshot.TryGetDouble(
        RIMDataId::kTemperatureSensorA,
        capability.temperature);

    snapshot.TryGetDouble(
        RIMDataId::kHumiditySensor,
        capability.humidity);

    store.Store(
        capability);
}

void BuildPrintReadyCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store)
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

    capability.ready =
        !upperDoorOpen &&
        !rightDoorOpen &&
        !leftDoorOpen;

    store.Store(
        capability);
}

void BuildConsumableCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store)
{
    ConsumableCapability capability{};

    snapshot.TryGetInt32(
        RIMDataId::kStapleLevel,
        capability.stapleLevel);

    snapshot.TryGetInt32(
        RIMDataId::kTonerLevel,
        capability.tonerLevel);

    store.Store(
        capability);
}

void BuildJobCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store)
{
    JobCapability capability{};

    snapshot.TryGetBool(
        RIMDataId::kJobActive,
        capability.jobActive);

    snapshot.TryGetInt32(
        RIMDataId::kJobId,
        capability.jobId);

    store.Store(
        capability);
}

} // namespace rim