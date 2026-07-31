#pragma once

#include "ISensorAdapter.hpp"

#include "rule/TemperatureSensorARule.hpp"

#include "RIMDataItem.hpp"

namespace rim
{

class TemperatureSensorAAdapter final
    : public ISensorAdapter
{
public:

    bool Store(
        const RIMValue& value,
        ValueStore& store) override
    {
        RIMDataItem item{};

        item.id =
            RIMDataId::kTemperatureSensorA;

        item.valueType =
            ValueType::kDouble;

        item.value =
            rule_.Execute(
                value);

        store.Store(item);

        return true;
    }

private:

    TemperatureSensorARule rule_;
};

} // namespace rim