#pragma once

#include <vector>

#include "ErrorInfo.hpp"

namespace rim
{

struct ErrorCapability
{
    std::vector<ErrorInfo>
        errors;
};

}