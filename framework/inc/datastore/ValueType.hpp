#pragma once

//
// ValueType - 正規化後の標準値のタグ。
//

namespace rim
{

enum class ValueType
{
    kNone = 0,
    kCelsiusX100,  // 固定小数: 摂氏×100
    kPercent,      // 0..100
    kJobProgress,  // 0..100
    kFaultCode,    // uint32
    kBool,         // 0/1
    kUInt32        // 汎用カウンタ等
};

} // namespace rim
