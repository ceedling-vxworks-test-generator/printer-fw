#pragma once

#include <cstddef>

namespace rim
{

struct BinaryInput
{
    const void* data{};

    std::size_t size{};
};

} // namespace rim