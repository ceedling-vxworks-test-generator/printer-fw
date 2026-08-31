#pragma once

#include <cstdint>
#include <vector>

namespace rim
{

struct SnapshotBinaryEntry
{
    std::vector<std::uint8_t> buffer;

    std::uint64_t hash{};
};

}