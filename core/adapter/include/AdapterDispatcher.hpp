#pragma once

#include "RIMDataItem.hpp"
#include "StoreInputQueue.hpp"
#include "ProductDefinition.hpp"

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

    // item.value は呼び出し側が item.id の DataItemDefinition::rawValueType
    // 通りに型付け済みであることを前提とする(型が一致しない場合は拒否してfalseを返す)。
    bool Dispatch(
        const RIMDataItem& item);

private:

    const ProductDefinition& product_;

    StoreInputQueue& queue_;
};

} // namespace rim