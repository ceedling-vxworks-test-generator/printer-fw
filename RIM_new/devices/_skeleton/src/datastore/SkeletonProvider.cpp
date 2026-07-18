#include "datastore/SkeletonProvider.hpp"

#include <vector>

#include "datastore/SensorDefinition.hpp"

namespace rim
{

const std::vector<RIMDataDefinition>&
SkeletonProvider::GetDefinitions() const
{
    //
    // TODO: この機種のデータ定義を列挙する。
    // 例: { RIMDataId::kTemperature, DataDomain::kDevice, ValueType::kDouble, "Temperature" }
    //
    static const std::vector<RIMDataDefinition> definitions =
    {
    };

    return definitions;
}

const std::vector<SensorDefinition>&
SkeletonProvider::GetSensorDefinitions() const
{
    //
    // TODO: この機種のセンサ定義(SensorId → RIMDataId → ValueType)を列挙する。
    //
    static const std::vector<SensorDefinition> definitions =
    {
    };

    return definitions;
}

} // namespace rim
