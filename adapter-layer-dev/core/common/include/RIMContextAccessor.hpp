#pragma once

//
// RIMContextAccessor - RIMContext(key,value)配列からキーを引く汎用ヘルパ。
//
// RIMContext自体(core/common/include/RIMContext.hpp)は0808の既存の型で、
// 「キー文字列+int値」を最大8件持てるだけの汎用的な入れ物。
// キーの読み方は使う側(機種依存のnormalize関数等)がバラバラに実装すると
// 表記揺れが起きるため、共通のルックアップ関数をここに置く。
//

#include <cstring>

#include "RIMContext.hpp"

namespace rim
{

// contextがnullptr、またはkeyが見つからない場合はfalseを返す(outValueは不変)。
inline bool TryGetContextValue(
    const RIMContext* context,
    const char* key,
    int& outValue)
{
    if (context == nullptr || key == nullptr)
    {
        return false;
    }

    for (const auto& entry : *context)
    {
        if (entry.key != nullptr &&
            std::strcmp(entry.key, key) == 0)
        {
            outValue = entry.value;

            return true;
        }
    }

    return false;
}

} // namespace rim
