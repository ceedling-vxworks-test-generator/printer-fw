#pragma once

#include "RIMContext.hpp"
#include "StoreInputQueue.hpp"
#include "ProductDefinition.hpp"
#include "rim_data_id.h"

namespace rim
{

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

    // IN: id(何のデータか) / value(生の数値) / context(単位などの補足情報。
    // 省略可、nullptr可 = 「既に正規化済み」として扱う)。
    // idとcontextの組でnormalize()(変換ルール)を辿り着けるようにする。
    bool Dispatch(
        RIDataId id,
        int value,
        const RIMContext* context = nullptr);

private:

    const ProductDefinition& product_;

    StoreInputQueue& queue_;
};

} // namespace rim
