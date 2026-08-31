#pragma once

namespace rim
{

class MachineCapabilityStore;
class RIMSnapshot;

void BuildEnvironmentCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store);

void BuildPrintReadyCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store);

void BuildConsumableCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store);

void BuildJobCapability(
    const RIMSnapshot& snapshot,
    MachineCapabilityStore& store);

}
