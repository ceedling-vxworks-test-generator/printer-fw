#pragma once

#include <cstdint>

#include "ErrorSeverity.hpp"

namespace rim
{

class IErrorDefinitionRegistry
{
public:

    virtual ~IErrorDefinitionRegistry()
        = default;

    virtual bool TryGetSeverity(
        std::uint32_t errorCode,
        ErrorSeverity& severity) const = 0;
};

} // namespace rim