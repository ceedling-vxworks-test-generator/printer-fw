#pragma once

#include "rim_data_id.h"

namespace rim
{

struct DeviceEvent
{
    RIDataId id;

    double value;

    // DataItemDefinition::normalize へそのまま渡される、id ごとに解釈が異なる
    // 補足情報(例: SourceUnit*)。省略時は nullptr(=正規化関数側の既定値を使う)。
    // 呼び出しの間だけ有効な非所有ポインタ。
    const void* context{nullptr};
};

} // namespace rim