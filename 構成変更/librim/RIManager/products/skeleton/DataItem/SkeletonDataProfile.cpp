#include "SkeletonDataProfile.hpp"

#include <cstddef>

#include "RIMValueFactory.hpp"

namespace rim
{
namespace
{

// TODO: この機種の正規化規則へ差し替えること。
// Rule は状態を持たないため、id 横断で共有する単一インスタンスとして持つ。
class PassThroughRule final : public IRule
{
public:

    std::optional<RIMValue> Convert(
        const RawValue& raw,
        const DataContext&) const override
    {
        if (raw.kind != RawValue::Kind::kScalar)
        {
            return std::nullopt;
        }

        return RIMValueFactory::CreateDouble(
            raw.scalar);
    }
};

const PassThroughRule kPassThrough;

struct Entry
{
    RIMDataId    id;
    const IRule* rule;
};

// RIMDataId の宣言順に並べ、id をそのまま添字にすること。
const Entry kTable[] = {
    { RIMDataId::kSampleValue, &kPassThrough },
};

// 行の足し忘れをビルドで検知する。
static_assert(
    sizeof(kTable) / sizeof(kTable[0]) ==
        static_cast<std::size_t>(kRIMDataIdCount),
    "kTable は RIMDataId 1件につき1行にすること");

} // namespace

const IRule* SkeletonDataProfile::SelectRule(
    RIMDataId id) const
{
    const int index = ToIndex(id);

    if (index < 0 || index >= kRIMDataIdCount)
    {
        return nullptr;
    }

    return kTable[index].rule;
}

} // namespace rim
