#pragma once

#include <cstddef>
#include <cstdint>

namespace rim
{

struct BinaryInfo
{
    const std::uint8_t* bytes{};

    std::size_t size{};

    std::uint64_t hash{};
};

} // namespace rim