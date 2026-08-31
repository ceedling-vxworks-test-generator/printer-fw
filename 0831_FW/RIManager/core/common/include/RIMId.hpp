#pragma once

#include <cstdint>
#include <functional>

#include "RIId.hpp"

namespace rim
{

constexpr std::uint32_t kInvalidRIMObjectId = 0xFFFFFFFFu;

enum class RIMIdType
{
    Data,
    Capability,
    Facade
};

struct RIMId
{
    RIMIdType type;
    std::uint32_t id{};

    constexpr bool
    operator==(const RIMId& rhs) const noexcept
    {
        return
            type == rhs.type &&
            id == rhs.id;
    }

    constexpr bool
    operator!=(const RIMId& rhs) const noexcept
    {
        return !(*this == rhs);
    }
};

struct RIMIdHash
{
    std::size_t operator()(
        const RIMId& id) const noexcept
    {
        const std::uint64_t key =
            (static_cast<std::uint64_t>(
                 id.type)
             << 32)
            | id.id;

        return std::hash<std::uint64_t>{}(
            key);
    }
};

} // namespace rim