#pragma once

//
// DataContext - 変換・反映の補助情報。rim_data_context_t 相当。
// rim の has_xxx フラグ方式は C++ の std::optional で表現する。
//   - faultState : 異常系Contextの状態(op未指定時にここから写像)
//   - scaleX1000 : スケール係数×1000(convert補助)
//   - op / key   : コレクション操作(管理配列への add/remove/update)
//

#include <cstdint>
#include <optional>

#include "datastore/FaultState.hpp"
#include "datastore/CollectionOp.hpp"

namespace rim
{

struct DataContext
{
    std::optional<FaultState>    faultState;
    std::optional<std::int32_t>  scaleX1000;
    std::optional<CollectionOp>  op;
    std::optional<std::uint32_t> key;
};

} // namespace rim
