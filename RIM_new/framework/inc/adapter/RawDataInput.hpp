#pragma once

//
// RawDataInput - RIM_AdapterLayer(L1)の受理点(仕様 §6/§8)。
// 型自由な PushI32/PushF/PushU32 を提供(内部で double へ集約)。
// RuleResolver.SelectRule → Rule.Convert(正規化) → Classify(性質判別) → CentralInputPort の3種post。
//

#include <cstdint>

#include "datastore/RIMDataId.hpp"
#include "datastore/DataContext.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/InputKind.hpp"
#include "datastore/IInputClassifier.hpp"
#include "datastore/DataStore.hpp"

#include "adapter/IRuleResolver.hpp"

namespace rim
{

class RawDataInput
{
public:

    RawDataInput(DataStore& store, const IRuleResolver& resolver, const IInputClassifier& classifier)
        : store_(store), resolver_(resolver), classifier_(classifier)
    {
    }

    RIMResult PushI32(RIMDataId id, std::int32_t raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, static_cast<double>(raw), ctx);
    }
    RIMResult PushF(RIMDataId id, double raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, raw, ctx);
    }
    RIMResult PushU32(RIMDataId id, std::uint32_t raw, const DataContext* ctx = nullptr)
    {
        return Intake(id, static_cast<double>(raw), ctx);
    }

private:

    RIMResult Intake(RIMDataId id, double raw, const DataContext* ctx)
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
            case InputKind::kFault:           return store_.PostFaultInput(item);
            case InputKind::kOperationReport: return store_.PostOperationReport(item);
            case InputKind::kCurrentValue:    return store_.PostCurrentValueInput(item);
            default:                          return RIMResult::kErrClassify;
        }
    }

    DataStore&              store_;
    const IRuleResolver&    resolver_;
    const IInputClassifier& classifier_;
};

} // namespace rim
