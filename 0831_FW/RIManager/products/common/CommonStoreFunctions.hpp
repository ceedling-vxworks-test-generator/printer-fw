#pragma once

#include "RIMValue.hpp"

namespace rim
{

inline RIMValue IdentityStore(
    const RIMValue&,
    const RIMValue& setValue,
    const void*)
{
    return setValue;
}

inline RIMValue StoreBinary(
    const RIMValue&,
    const RIMValue& setValue,
    const void*)
{
    return setValue;
}

} // namespace rim