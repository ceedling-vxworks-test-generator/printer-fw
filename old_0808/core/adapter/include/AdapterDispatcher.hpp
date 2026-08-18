#pragma once

#include "rim_data_id.h"

#include "RIMValue.hpp"
#include "StoreInputQueue.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

// Adapterレイヤの受付窓口。
// 「RIDataIdごとに定義された正しい型のRIMValue」を受け取り、
// DataItemDefinition::normalizeで正規化した上でStoreレイヤ(StoreInputQueue)へ
// 引き渡す。製品固有の知識(PrinterA/センサー名など)は一切持たず、
// ProductDefinitionを介してのみDataItemの型・正規化規則を参照する。
class AdapterDispatcher
{
public:

    AdapterDispatcher(
        const ProductDefinition& product,
        StoreInputQueue& queue)
        : product_(product),
          queue_(queue)
    {
    }

    // value は呼び出し側が id の DataItemDefinition::rawValueType 通りに
    // 型付け済みであることを前提とする(型が一致しない場合は拒否してfalseを返す)。
    bool Dispatch(
        RIDataId id,
        const RIMValue& value);

private:

    const ProductDefinition& product_;

    StoreInputQueue& queue_;
};

} // namespace rim