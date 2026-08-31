#pragma once

#include <cstdint>
#include <string_view>

namespace rim
{

class ProductInfo
{
public:

    static constexpr std::string_view
    GetName() noexcept
    {
        return "Test Product";
    }
};

#define RIM_PRODUCT_VERSION_MAJOR 2
#define RIM_PRODUCT_VERSION_MINOR 3
#define RIM_PRODUCT_VERSION_PATCH 4

#define RIM_PRODUCT_STRINGIZE_IMPL(x) #x
#define RIM_PRODUCT_STRINGIZE(x) RIM_PRODUCT_STRINGIZE_IMPL(x)

#define RIM_PRODUCT_VERSION_STRING \
    RIM_PRODUCT_STRINGIZE(RIM_PRODUCT_VERSION_MAJOR) "." \
    RIM_PRODUCT_STRINGIZE(RIM_PRODUCT_VERSION_MINOR) "." \
    RIM_PRODUCT_STRINGIZE(RIM_PRODUCT_VERSION_PATCH)

struct RIMProductVersionInfo
{
    std::uint16_t major;
    std::uint16_t minor;
    std::uint16_t patch;
};

class ProductVersionInfo
{
public:
    static constexpr RIMProductVersionInfo Get() noexcept
    {
        return {
            RIM_PRODUCT_VERSION_MAJOR,
            RIM_PRODUCT_VERSION_MINOR,
            RIM_PRODUCT_VERSION_PATCH
        };
    }

    static constexpr std::string_view GetString() noexcept
    {
        return kVersionString;
    }

private:
    static constexpr std::string_view kVersionString =
        RIM_PRODUCT_VERSION_STRING;
};

}