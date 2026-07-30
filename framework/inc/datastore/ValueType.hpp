#pragma once

//
// ValueType - 正規化後の標準値のタグ。
//

namespace rim
{

enum class ValueType
{
    kNone = 0,
    kKelvinX100,   // 固定小数: 絶対温度(ケルビン)×100。温度の内部統一表現
                   // (入力が摂氏/華氏でも Adapter でここへ正規化される)
    kPercent,      // 0..100
    kJobProgress,  // 0..100
    kFaultCode,    // uint32
    kBool,         // 0/1
    kUInt32        // 汎用カウンタ等
};

} // namespace rim
