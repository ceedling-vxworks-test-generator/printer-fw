#pragma once

#include <cstdint>

#include "ValueType.hpp"

namespace rim
{

union RIMValueStorage
{
    bool b;

    std::int32_t i32;
    std::uint32_t u32;

    std::int64_t i64;
    std::uint64_t u64;

    double d;

    const char* str;

    void* ptr;
};

struct RIMValue
{
    ValueType type{};

    RIMValueStorage value{};
};

} // namespace rim
