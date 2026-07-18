#pragma once

//
// ValueType - 正規化後の標準値のタグ。rim_value_type_t 相当。
//

namespace rim
{

enum class ValueType
{
    kNone = 0,
    kCelsiusX100,  // 固定小数: 摂氏×100
    kPercent,      // 0..100
    kJobProgress,  // 0..100
    kFaultCode
};

} // namespace rim
