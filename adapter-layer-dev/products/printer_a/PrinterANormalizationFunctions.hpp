#pragma once

#include "RIMContext.hpp"
#include "RIMValue.hpp"

namespace rim
{

// 温度: context(kContextKeyUnit)で単位を判別し、絶対温度(ケルビン)へ正規化する。
//   未指定 / kUnitNormalized : 既にケルビンとみなしてそのまま採用
//   kUnitCelsius             : K = C + 273.15
//   kUnitFahrenheit          : K = (F - 32) * 5/9 + 273.15
RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const RIMContext* context);

// ステープルレベル: 0〜100へクランプし、int32へ変換する(単位なし)。
RIMValue NormalizeStapleLevel(
    const RIMValue& value,
    const RIMContext* context);

} // namespace rim
