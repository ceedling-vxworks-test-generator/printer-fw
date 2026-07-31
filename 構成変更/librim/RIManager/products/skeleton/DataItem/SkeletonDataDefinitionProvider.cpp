#include "SkeletonDataDefinitionProvider.hpp"

#include <vector>

#include "DataDomain.hpp"
#include "RIMDataId.hpp"
#include "ValueType.hpp"

namespace rim
{

const std::vector<RIMDataDefinition>&
SkeletonDataDefinitionProvider::GetDefinitions() const
{
    // TODO: この機種が扱う DataItem を列挙する(下記は雛形の1件のみ)。
    static const std::vector<RIMDataDefinition>
    definitions =
    {
        {
            RIMDataId::kTemperatureSensorA,
            DataDomain::kDevice,
            ValueType::kDouble,
            "Temperature"
        }
    };

    return definitions;
}

} // namespace rim
