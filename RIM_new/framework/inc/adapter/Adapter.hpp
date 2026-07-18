#pragma once

//
// Adapter - L1 RIM_AdapterLayer(C++ OO 移植)。
// rim_adapter.cpp の移植。受理点の「型の自由さ」は PushI32/PushF/PushU32 で表現し、
// 内部 Intake で double へ集約 → IDataProfile::Convert(正規化) → Classify(性質判別)
// → DataStore の3種postへ振り分ける。Submit はコレクション操作(add/remove/update)。
//

#include <cstdint>

#include "datastore/RIMDataId.hpp"
#include "datastore/RIMValue.hpp"
#include "datastore/DataContext.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/InputKind.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/DataStore.hpp"

#include "adapter/IDataProfile.hpp"

namespace rim
{

class Adapter
{
public:

    Adapter(DataStore& store, const IDataProfile& profile)
        : store_(store)
        , profile_(profile)
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

    // コレクション操作の受理点(FAULT/OPERATION レーン限定)。
    RIMResult Submit(RIMDataId id, CollectionOp op, std::uint32_t key,
                     const RIMValue* value = nullptr, const DataContext* ctx = nullptr);

private:

    RIMResult Intake(RIMDataId id, double raw, const DataContext* ctx);
    RIMResult DispatchPost(InputKind kind, const RIMDataItem& item);

    DataStore&          store_;
    const IDataProfile& profile_;
};

} // namespace rim
