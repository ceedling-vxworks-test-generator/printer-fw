#pragma once

//
// RawDataInput - RIM_AdapterLayer(L1)の受理点(仕様 §6/§8)。
// 型自由な受理点を提供する:
//   - スカラ  : PushI32 / PushF / PushU32(内部で double へ集約)
//   - 構造体  : PushStruct<T>(id, const T&)
//   - 配列    : PushArray<T>(id, const T*, n)
// 流れ: RawValue 生成 → RuleResolver.SelectRule → Rule.Convert(正規化) →
//       Classify(性質判別) → ICentralInputPort の3種post。
// Adapter は ICentralInputPort にのみ依存する(具象 DataStore 非依存 → 単体テスト可)。
//

#include <cstddef>
#include <cstdint>

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

    // --- スカラ受理点 ---
    RIMResult PushI32(RIMDataId id, std::int32_t raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, RawValue::Scalar(static_cast<double>(raw)), ctx);
    }
    RIMResult PushF(RIMDataId id, double raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, RawValue::Scalar(raw), ctx);
    }
    RIMResult PushU32(RIMDataId id, std::uint32_t raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, RawValue::Scalar(static_cast<double>(raw)), ctx);
    }

    // --- 構造体受理点 ---
    template <typename T>
    RIMResult PushStruct(RIMDataId id, const T& raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, RawValue::Struct(raw), ctx);
    }

    // --- 配列受理点 ---
    template <typename T>
    RIMResult PushArray(RIMDataId id, const T* raw, std::size_t n, const DataContext* ctx = nullptr)
    {
        return Intake(id, RawValue::Array(raw, n), ctx);
    }

private:

    RIMResult Intake(RIMDataId id, const RawValue& raw, const DataContext* ctx)
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

    ICentralInputPort&      port_;
    const IRuleResolver&    resolver_;
    const IInputClassifier& classifier_;
};

} // namespace rim
