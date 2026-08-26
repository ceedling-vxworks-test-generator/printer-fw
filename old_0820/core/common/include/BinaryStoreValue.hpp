#pragma once

#include <cstdint>
#include <vector>

namespace rim
{

struct BinaryStoreValue
{
    std::vector<std::uint8_t> data;

    std::uint64_t hash{};
};

} // namespace rim