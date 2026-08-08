#pragma once

#include <any>
#include <new>

namespace rim
{

class RIMSnapshot;

std::any BuildEnvironmentCapability(
    const RIMSnapshot& snapshot);

std::any BuildPrintReadyCapability(
    const RIMSnapshot& snapshot);

std::any BuildConsumableCapability(
    const RIMSnapshot& snapshot);

std::any BuildJobCapability(
    const RIMSnapshot& snapshot);

}
