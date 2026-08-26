#pragma once

#include <cstdint>
#include <string_view>

namespace rim
{

#define RIM_CORE_VERSION_MAJOR 1
#define RIM_CORE_VERSION_MINOR 0
#define RIM_CORE_VERSION_PATCH 0

#define RIM_CORE_STRINGIZE_IMPL(x) #x
#define RIM_CORE_STRINGIZE(x) RIM_CORE_STRINGIZE_IMPL(x)

#define RIM_CORE_VERSION_STRING \
    RIM_CORE_STRINGIZE(RIM_CORE_VERSION_MAJOR) "." \
    RIM_CORE_STRINGIZE(RIM_CORE_VERSION_MINOR) "." \
    RIM_CORE_STRINGIZE(RIM_CORE_VERSION_PATCH)

struct RIMCoreVersionInfo
{
    std::uint16_t major;
    std::uint16_t minor;
    std::uint16_t patch;
};

class CoreVersionInfo
{
public:
    static constexpr RIMCoreVersionInfo Get() noexcept
    {
        return {
            RIM_CORE_VERSION_MAJOR,
            RIM_CORE_VERSION_MINOR,
            RIM_CORE_VERSION_PATCH
        };
    }

    static constexpr std::string_view GetString() noexcept
    {
        return kVersionString;
    }

private:
    static constexpr std::string_view kVersionString =
        RIM_CORE_VERSION_STRING;
};

}