#pragma once

#include "IErrorDefinitionRegistry.hpp"

namespace rim
{

class SkeletonErrorRegistry final
    : public IErrorDefinitionRegistry
{
public:

    bool TryGetSeverity(
        std::uint32_t errorCode,
        ErrorSeverity& severity) const override;
};

} // namespace rim
`