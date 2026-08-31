#pragma once

#include "RIMValue.hpp"

namespace rim
{

class RIMSnapshot;

RIMValue BuildProductState(const RIMSnapshot& snapshot);
RIMValue BuildSupplyStatus(const RIMSnapshot& snapshot);
RIMValue BuildPrintJobStatus(const RIMSnapshot& snapshot);

}
