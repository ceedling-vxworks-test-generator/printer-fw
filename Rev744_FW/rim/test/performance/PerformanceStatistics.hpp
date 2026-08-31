#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

namespace perf
{

struct Statistics
{
    int64_t min{};

    int64_t avg{};

    int64_t p95{};

    int64_t p99{};

    int64_t max{};
};

inline Statistics
CalculateStatistics(
    std::vector<int64_t> values)
{
    Statistics stats{};

    if (values.empty())
    {
        return stats;
    }

    std::sort(
        values.begin(),
        values.end());

    stats.min =
        values.front();

    stats.max =
        values.back();

    stats.avg =
        std::accumulate(
            values.begin(),
            values.end(),
            int64_t{0})
        / values.size();

    const auto p95Index =
        static_cast<std::size_t>(
            std::ceil(
                values.size() * 0.95))
        - 1U;

    const auto p99Index =
        static_cast<std::size_t>(
            std::ceil(
                values.size() * 0.99))
        - 1U;

    stats.p95 =
        values[p95Index];

    stats.p99 =
        values[p99Index];

    return stats;
}

} // namespace perf
