#pragma once

#include <cstdint>
#include <vector>

#include "BinaryHash.hpp"

namespace rim
{

struct BinaryEntry
{
    void Set(
        const std::uint8_t* data,
        std::size_t size)
    {
        buffer.assign(
            data,
            data + size);

        hash =
            CalculateBinaryHash(
                buffer.data(),
                buffer.size());
    }

    std::vector<std::uint8_t>
        buffer;

    std::uint64_t hash{};
};

} // namespace rim