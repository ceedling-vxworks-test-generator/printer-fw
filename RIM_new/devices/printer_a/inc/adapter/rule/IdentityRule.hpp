#pragma once

#include "adapter/rule/ISensorRule.hpp"

namespace rim
{

class IdentityRule final
    : public ISensorRule
{
public:

    RIMValue Execute(
        const RIMValue& input) const override
    {
        return input;
    }
};

} // namespace rim