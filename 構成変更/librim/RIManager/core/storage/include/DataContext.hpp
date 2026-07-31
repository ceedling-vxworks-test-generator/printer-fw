#pragma once

//
// DataContext - データに付与される補足情報(仕様: Context)。has_xxx は std::optional で表現。
//   unit       : 生値の単位/表現(Rule が正規化方法を分岐するのに使う)
//   faultState : 異常系の状態(FaultRegistry.apply の分岐に使用)
//   scaleX1000 : スケール係数×1000(Rule変換の補助)
//   key        : Faultコレクションのキー(fault code等)
//

#include <cstdint>
#include <optional>

#include "FaultState.hpp"
#include "SourceUnit.hpp"

namespace rim
{

struct DataContext
{
    // 同一 id へ異なる単位で値が届く場合に、どの単位で来たかを示す。
    // 未指定は SourceUnit::kNormalized(既に正規化済み=変換しない)と同義。
    std::optional<SourceUnit>    unit;
    std::optional<FaultState>    faultState;
    std::optional<std::int32_t>  scaleX1000;
    std::optional<std::uint32_t> key;
};

} // namespace rim
