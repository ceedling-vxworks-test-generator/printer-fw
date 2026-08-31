#pragma once

#include "RIMValue.hpp"

namespace rim
{

inline bool DefaultDiff(
    const RIMValue& lhs,
    const RIMValue& rhs)
{
    return std::memcmp(
        &lhs,
        &rhs,
        sizeof(RIMValue)) != 0;
}

}