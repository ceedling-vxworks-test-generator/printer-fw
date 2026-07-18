#pragma once

#include "adapter/ISensorAdapter.hpp"

#include "adapter/rule/TemperatureSensorARule.hpp"

#include "datastore/RIMDataItem.hpp"

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

    TemperatureSensorARule rule_;
};

} // namespace rim