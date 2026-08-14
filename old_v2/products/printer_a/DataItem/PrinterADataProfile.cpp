#include "PrinterADataProfile.hpp"

#include <cstddef>

#include "PrinterARules.hpp"

namespace rim
{
namespace
{

// Rule 実体は状態を持たないため、id 横断で共有する単一インスタンスとして保持する。
const TemperatureRule kTemperature;
const PercentRule     kPercent;
const BoolRule        kBoolean;
const CountRule       kCount;

struct Entry
{
    RIMDataId    id;    // 自己文書化・試験用(検索には使わない。添字は id そのもの)
    const IRule* rule;
};

//
// id -> Rule の対応表。RIMDataId 1件につきこの1行で完結する。
// RIMDataId の宣言順に並べ、id をそのまま添字にする(O(1)、稠密 ID 規約)。
//
const Entry kTable[] = {
    { RIMDataId::kTemperatureSensorA, &kTemperature },
    { RIMDataId::kTemperatureSensorB, &kTemperature },
    { RIMDataId::kHumiditySensor,     &kPercent     },
    { RIMDataId::kUpperDoorOpen,      &kBoolean     },
    { RIMDataId::kRightDoorOpen,      &kBoolean     },
    { RIMDataId::kLeftDoorOpen,       &kBoolean     },
    { RIMDataId::kStapleLevel,        &kPercent     },
    { RIMDataId::kTonerLevel,         &kPercent     },
    { RIMDataId::kJobActive,          &kBoolean     },
    { RIMDataId::kJobId,              &kCount       },
};

//
// 行数と RIMDataId の総数がずれたらビルドで検知する。
// switch の default: が握り潰していた網羅性チェックの代わりであり、
// **RIMDataId に列挙子を足して表に行を足し忘れると、その場でビルドが落ちる**。
// 並び崩れ(行の取り違え)は PrinterADataProfileTest が検知する。
//
static_assert(
    sizeof(kTable) / sizeof(kTable[0]) ==
        static_cast<std::size_t>(kRIMDataIdCount),
    "kTable は RIMDataId 1件につき1行にすること"
    "(RIMDataId を増減したら行も増減する)");

} // namespace

const IRule* PrinterADataProfile::SelectRule(
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
