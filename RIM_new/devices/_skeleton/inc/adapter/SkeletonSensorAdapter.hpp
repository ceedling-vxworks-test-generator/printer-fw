#pragma once

#include "adapter/ISensorAdapter.hpp"

#include "adapter/rule/SkeletonSensorRule.hpp"

#include "datastore/RIMDataItem.hpp"

namespace rim
{

//
// TODO: Skeleton を機種/センサ名に置換。
// センサ値を RIMDataItem へ変換して ValueStore へ格納する具象 Adapter。
//
class SkeletonSensorAdapter final
    : public ISensorAdapter
{
public:

    bool Store(
        const RIMValue& value,
        ValueStore& store) override
    {
        RIMDataItem item{};

        // TODO: item.id / item.valueType を機種のセンサに合わせて設定。
        // item.id        = RIMDataId::kTemperature;
        // item.valueType = ValueType::kDouble;

        item.value =
            rule_.Execute(
                value);

        store.Store(item);

        return true;
    }

private:

    SkeletonSensorRule rule_;
};

} // namespace rim
