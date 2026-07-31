#pragma once

#include "RIMValue.hpp"

namespace rim
{

class ISensorRule
{
public:

    virtual ~ISensorRule() = default;

    virtual RIMValue Execute(
        const RIMValue& input) const = 0;
};

} // namespace rim