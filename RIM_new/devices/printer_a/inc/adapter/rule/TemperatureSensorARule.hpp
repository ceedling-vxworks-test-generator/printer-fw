#pragma once

#include "adapter/rule/ISensorRule.hpp"

#include "datastore/RIMValueAccessor.hpp"
#include "datastore/RIMValueFactory.hpp"

namespace rim
{

class TemperatureSensorARule final
    : public ISensorRule
{
public:

    RIMValue Execute(
        const RIMValue& input) const override
    {
        double celsius{};

        if (!RIMValueAccessor::GetDouble(
                input,
                celsius))
        {
            return {};
        }

        return
            RIMValueFactory::CreateDouble(
                celsius + 273.15);
    }
};

} // namespace rim