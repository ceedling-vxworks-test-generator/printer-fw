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
    // 参照する id は **同じ products/<機種>/DataItem/RIMDataId.hpp のもの**にすること
    // (他機種の id を書くと、その機種を差し替えたときにビルドが落ちる)。
    static const std::vector<RIMDataDefinition>
    definitions =
    {
        {
            RIMDataId::kSampleValue,
            DataDomain::kDevice,
            ValueType::kDouble,
            "SampleValue"
        }
    };

    return definitions;
}

} // namespace rim
