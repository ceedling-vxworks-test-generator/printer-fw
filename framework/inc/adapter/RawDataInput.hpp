#pragma once

//
// RawDataInput - RIM_AdapterLayer(L1)の受理点(仕様 §6/§8)。
// 外部へ提供する受理点は Push(id, RawValue, ctx) の1つのみ。型自由さは RawValue 側の
// タグ(kind)が吸収し、呼出側は RawValue::Scalar/Struct/Array で組み立てて渡す:
//   - スカラ  : Push(id, RawValue::Scalar(v))
//   - 構造体  : Push(id, RawValue::Struct(s))
//   - 配列    : Push(id, RawValue::Array(a, n))
// 流れ: RuleResolver.SelectRule → Rule.Convert(正規化) →
//       Classify(性質判別) → ICentralInputPort の3種post。
// Adapter は ICentralInputPort にのみ依存する(具象 DataStore 非依存 → 単体テスト可)。
//

#include "adapter/RawValue.hpp"
#include "adapter/IRuleResolver.hpp"

#include "datastore/RIMDataId.hpp"
#include "datastore/DataContext.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/InputKind.hpp"
#include "datastore/IInputClassifier.hpp"
#include "datastore/ICentralInputPort.hpp"

namespace rim
{

class RawDataInput
{
public:

    RawDataInput(ICentralInputPort& port, const IRuleResolver& resolver, const IInputClassifier& classifier)
        : port_(port), resolver_(resolver), classifier_(classifier)
    {
    }

    // --- 唯一の受理点 ---
    // raw は RawValue::Scalar/Struct/Array で組み立てる(型自由さは RawValue が吸収する)。
    RIMResult Push(RIMDataId id, const RawValue& raw, const DataContext* ctx = nullptr)
    {
        const IRule* rule = resolver_.SelectRule(id);
        if (rule == nullptr) return RIMResult::kErrConvert;

        RIMDataItem item;
        item.id = id;
        if (ctx) item.context = *ctx;

        auto value = rule->Convert(raw, item.context);
        if (!value) return RIMResult::kErrConvert;
        item.value = *value;

        auto kind = classifier_.Classify(id);
        if (!kind) return RIMResult::kErrClassify;

        switch (*kind) {
            case InputKind::kFault:           return port_.PostFaultInput(item);
            case InputKind::kOperationReport: return port_.PostOperationReport(item);
            case InputKind::kCurrentValue:    return port_.PostCurrentValueInput(item);
            default:                          return RIMResult::kErrClassify;
        }
    }

private:

    ICentralInputPort&      port_;
    const IRuleResolver&    resolver_;
    const IInputClassifier& classifier_;
};

} // namespace rim
