#pragma once

#include <cstring>

#include "RIMValue.hpp"

namespace rim
{

inline bool IdentityDiff(
    const RIMValue& lhs,
    const RIMValue& rhs)
{
    if (lhs.type != rhs.type)
    {
        return true;
    }

    switch (lhs.type)
    {
    case ValueType::kBool:
        return lhs.value.b != rhs.value.b;

    case ValueType::kInt32:
        return lhs.value.i32 != rhs.value.i32;

    case ValueType::kUInt32:
        return lhs.value.u32 != rhs.value.u32;

    case ValueType::kInt64:
        return lhs.value.i64 != rhs.value.i64;

    case ValueType::kUInt64:
        return lhs.value.u64 != rhs.value.u64;

    case ValueType::kDouble:
        return lhs.value.d != rhs.value.d;

    case ValueType::kString:
        return lhs.value.str != rhs.value.str;

    case ValueType::kBinary:
    {
        if (lhs.size != rhs.size)
        {
            return true;
        }

        return std::memcmp(
                lhs.value.bytes,
                rhs.value.bytes,
                lhs.size) != 0;
    }

    default:
        return true;
    }
}

} // namespace rim