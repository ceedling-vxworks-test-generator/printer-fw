#include "adapter/PrinterADataProfile.hpp"

#include <cstddef>

namespace rim
{
namespace
{

// Rule実体は状態を持たないため、id横断で共有する単一インスタンスとして保持する。
const TemperatureRule kTemperature;
const PercentRule   kPercent;
const ProgressRule  kProgress;
const FaultCodeRule kFaultCode;
const BoolRule      kBoolean;
const CountRule     kCount;

struct Entry
{
    RIMDataId    id;    // 自己文書化・テスト用(検索には使わない。添字は id そのもの)
    const IRule* rule;
    InputKind    kind;
};

// id → (Rule, 性質) の対応表。RIMDataId 1件につきこの1行のみで完結する
// (旧: SelectRule/Classify 2つのswitchに分かれていたものを統合)。
// RIMDataId の宣言順に並べ、id をそのまま添字にする(O(1)、稠密ID規約。
// framework の CurrentValueBuffer/Registry と同じ流儀)。
const Entry kTable[] = {
    { RIMDataId::kTemperature,      &kTemperature, InputKind::kCurrentValue },
    { RIMDataId::kHumidity,         &kPercent,   InputKind::kCurrentValue },
    { RIMDataId::kPressure,         &kPercent,   InputKind::kCurrentValue },
    { RIMDataId::kInkLevel,         &kPercent,   InputKind::kCurrentValue },
    { RIMDataId::kWiperLevel,       &kPercent,   InputKind::kCurrentValue },
    { RIMDataId::kCoverOpen,        &kBoolean,   InputKind::kCurrentValue },
    { RIMDataId::kEStop,            &kBoolean,   InputKind::kCurrentValue },
    { RIMDataId::kJobProgress,      &kProgress,  InputKind::kOperationReport },
    { RIMDataId::kFaultCode,        &kFaultCode, InputKind::kFault },
    { RIMDataId::kMaintenanceCount, &kCount,     InputKind::kCurrentValue },
    { RIMDataId::kUnitAlive,        &kBoolean,   InputKind::kCurrentValue },
};

// 行数と RIMDataId の総数がずれたらビルドで検知する(switchの default: が消していた
// 網羅性チェックの代わり)。並び崩れ(行の取り違え)は PrinterADataProfileTest が検知する。
static_assert(sizeof(kTable) / sizeof(kTable[0]) == static_cast<std::size_t>(kRIMDataIdCount),
              "kTable must have exactly one row per RIMDataId (add/remove a row when RIMDataId changes)");

} // namespace

const IRule* PrinterADataProfile::SelectRule(RIMDataId id) const
{
    const int idx = ToIndex(id);
    if (idx < 0 || idx >= kRIMDataIdCount) return nullptr;
    return kTable[idx].rule;
}

std::optional<InputKind> PrinterADataProfile::Classify(RIMDataId id) const
{
    const int idx = ToIndex(id);
    if (idx < 0 || idx >= kRIMDataIdCount) return std::nullopt;
    return kTable[idx].kind;
}

} // namespace rim
