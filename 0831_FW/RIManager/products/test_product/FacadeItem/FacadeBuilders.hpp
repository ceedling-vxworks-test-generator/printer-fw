#pragma once

#include "RIMValue.hpp"

namespace rim
{

class RIMSnapshot;

RIMValue BuildEnvironmentReadyFacade(const RIMSnapshot& snapshot);
RIMValue BuildOperationReadyFacade(const RIMSnapshot& snapshot);

}
