#pragma once

//
// RawDataInput - Adapter 層(L1)の受理点。
//
// 外部へ提供する受理点は Push(id, RawValue, ctx) の **1本のみ**。
// 型の自由さは RawValue のタグ(kind)が吸収する:
//   - スカラ  : Push(id, RawValue::Scalar(v))
//   - 構造体  : Push(id, RawValue::Struct(s))
//   - 配列    : Push(id, RawValue::Array(a, n))
//
// 流れ: IRuleResolver::SelectRule -> IRule::Convert(正規化) -> ICentralInputPort::Post
//
// Adapter の責務は「正規化して渡す」ことだけである。入力性質の判別やレーン
// 振り分けは L2 の責務なので、ここでは行わない。
// 依存先は ICentralInputPort という抽象のみなので、単体試験ではモックを差せる。
//
// 単位の扱い: 同じ id に摂氏で来ても華氏で来ても、どの単位で来たかを
// DataContext::unit で受け、規則が内部統一表現へ正規化する
// (id を単位ごとに増やさない)。
//

#include "DataContext.hpp"
#include "ICentralInputPort.hpp"
#include "IRuleResolver.hpp"
#include "RIMDataId.hpp"
#include "RIMDataItem.hpp"
#include "RIMResult.hpp"
#include "RawValue.hpp"

namespace rim
{

class RawDataInput
{
public:

    RawDataInput(
        ICentralInputPort& port,
        const IRuleResolver& resolver)
        : port_(port)
        , resolver_(resolver)
    {
    }

    // --- 唯一の受理点 ---
    RIMResult Push(
        RIMDataId id,
        const RawValue& raw,
        const DataContext* ctx = nullptr)
    {
        const IRule* rule =
            resolver_.SelectRule(id);

        if (rule == nullptr)
        {
            return RIMResult::kErrConvert;
        }

        RIMDataItem item;

        item.id = id;

        if (ctx != nullptr)
        {
            item.context = *ctx;
        }

        auto value =
            rule->Convert(
                raw,
                item.context);

        if (!value)
        {
            return RIMResult::kErrConvert;
        }

        item.value = *value;

        return port_.Post(
            item);
    }

private:

    ICentralInputPort&   port_;
    const IRuleResolver& resolver_;
};

} // namespace rim
