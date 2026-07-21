#pragma once

//
// DataContext - データに付与される補足情報(仕様: Context)。has_xxx は std::optional で表現。
//   faultState : 異常系の状態(FaultRegistry.apply の分岐に使用)
//   scaleX1000 : スケール係数×1000(Rule変換の補助)
//   key        : Faultコレクションのキー(fault code等)
//

#include <cstdint>
#include <optional>

#include "datastore/FaultState.hpp"

namespace rim
{

struct DataContext
{
    std::optional<FaultState>    faultState;
    std::optional<std::int32_t>  scaleX1000;
    std::optional<std::uint32_t> key;
};

} // namespace rim
