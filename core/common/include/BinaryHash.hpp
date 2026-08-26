#pragma once

#include <cstddef>
#include <cstdint>

namespace rim
{

inline std::uint64_t CalculateBinaryHash(
    const std::uint8_t* data,
    std::size_t size)
{
    constexpr std::uint64_t kOffset =
        14695981039346656037ULL;

    constexpr std::uint64_t kPrime =
        1099511628211ULL;

    std::uint64_t hash =
        kOffset;

    for (std::size_t i = 0;
         i < size;
         ++i)
    {
        hash ^= data[i];
        hash *= kPrime;
    }

    return hash;
}

} // namespace rim