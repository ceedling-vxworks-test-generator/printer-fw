#pragma once

#include "adapter/ISensorAdapter.hpp"

#include "adapter/rule/IdentityRule.hpp"

namespace rim
{

class TemperatureSensorBAdapter final
    : public ISensorAdapter
{
public:

    bool Store(
        const RIMValue& value,
        ValueStore& store) override
    {
        RIMDataItem item{};

        item.id =
            RIMDataId::kTemperature;

        item.valueType =
            ValueType::kDouble;

        item.value =
            rule_.Execute(
                value);

        store.Store(item);

        return true;
    }

private:

    IdentityRule rule_;
};

} // namespace rim