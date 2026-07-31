#pragma once

//
// RIMDataItem - 層間で受け渡す標準データモデル {id, value, context}。
//
// context は Adapter が受け取った補足情報(単位・異常状態・スケール等)をそのまま
// 後段へ運ぶためのもの。規則が正規化に使うほか、L2 が異常レーンの分岐に使う。
//

#include "DataContext.hpp"
#include "RIMDataId.hpp"
#include "RIMValue.hpp"

namespace rim
{

struct RIMDataItem
{
    RIMDataId   id{};
    ValueType   valueType{};
    RIMValue    value{};
    DataContext context{};
};

} // namespace rim
