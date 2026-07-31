#pragma once

//
// PrinterADataProfile - PrinterA の id 定義(機種可変)。
//
// IRuleResolver を実装し、id -> Rule の対応を **テーブル駆動** で持つ。
// 旧実装は switch 文だったが、規則が100〜1000件に増えると
//   - 行が増えるほど分岐が伸びる
//   - default: があるため「追加し忘れ」がビルドで検出できない
// という問題があるため、id を添字にする表へ置き換えてある(参照は O(1))。
//
// Rule 実体は状態を持たないため、id 横断で共有する単一インスタンスとして持つ
// (このクラスは表の参照窓口であり、Rule を所有しない)。
//

#include "IRuleResolver.hpp"

namespace rim
{

class PrinterADataProfile final
    : public IRuleResolver
{
public:

    const IRule* SelectRule(
        RIMDataId id) const override;
};

} // namespace rim
