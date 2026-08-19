#pragma once

#include "RIMDataItem.hpp"
#include "RouteProvider.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class AdapterDispatcher
{
public:

    AdapterDispatcher(
        const ProductDefinition& product,
        RouteProvider& routeProvider)
        : product_(product),
          routeProvider_(routeProvider)
    {
    }

    // item.value は呼び出し側が item.id の DataItemDefinition::rawValueType
    // 通りに型付け済みであることを前提とする(型が一致しない場合は拒否してfalseを返す)。
    bool Dispatch(
        const RIMDataItem& item);

private:

    const ProductDefinition& product_;

    RouteProvider& routeProvider_;
};

} // namespace rim