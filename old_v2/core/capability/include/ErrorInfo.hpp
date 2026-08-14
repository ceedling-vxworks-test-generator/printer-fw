#pragma once

#include <cstdint>

#include "ErrorSeverity.hpp"

namespace rim
{

enum class ErrorState
{
    kActive,
    kRecovered
};

struct ErrorInfo
{
    std::uint32_t errorCode{};

    ErrorState state{};

    ErrorSeverity severity{
        ErrorSeverity::kError
    };

    bool operator==(
        const ErrorInfo& rhs) const
    {
        return errorCode ==
                   rhs.errorCode
               &&
               state ==
                   rhs.state
               &&
               severity ==
                   rhs.severity;
    }
};

}