#pragma once

#include "RIMValue.hpp"

namespace rim
{

class RIMSnapshot;

RIMValue BuildEnvironmentCapability(const RIMSnapshot& snapshot);
RIMValue BuildPrintReadyCapability(const RIMSnapshot& snapshot);

}
