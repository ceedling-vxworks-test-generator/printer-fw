#pragma once

#include <vector>

#include "datastore/RIMDataDefinition.hpp"
#include "datastore/IDataDefinitionProvider.hpp"
#include "datastore/SensorDefinition.hpp"

namespace rim
{

//
// TODO: Skeleton を機種名(例 PrinterB)に置換。
// この機種が公開するデータ定義とセンサ定義を返す。
//
class SkeletonProvider
    : public IDataDefinitionProvider
{
public:

    const std::vector<RIMDataDefinition>&
    GetDefinitions() const override;

    const std::vector<SensorDefinition>&
    GetSensorDefinitions() const;
};

} // namespace rim
