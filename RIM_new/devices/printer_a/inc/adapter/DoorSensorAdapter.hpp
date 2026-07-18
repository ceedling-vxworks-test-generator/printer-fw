#pragma once

#include "adapter/ISensorAdapter.hpp"

#include "adapter/rule/IdentityRule.hpp"

namespace rim
{

class DoorSensorAdapter final
    : public ISensorAdapter
{
public:

    explicit DoorSensorAdapter(
        RIMDataId doorId)
        : doorId_(doorId)
    {
    }

    bool Store(
        const RIMValue& value,
        ValueStore& store) override
    {
        RIMDataItem item{};

        item.id =
            doorId_;

        item.valueType =
            ValueType::kBool;

        item.value =
            rule_.Execute(
                value);

        store.Store(item);

        return true;
    }

private:

    RIMDataId doorId_;

    IdentityRule rule_;
};

} // namespace rim