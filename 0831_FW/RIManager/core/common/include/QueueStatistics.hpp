#pragma once

#include <atomic>
#include <cstdint>

namespace rim
{

struct QueueStatistics
{
    std::atomic_uint64_t pushed {};
    std::atomic_uint64_t compressed {};
    std::atomic_uint64_t delivered {};
};

}