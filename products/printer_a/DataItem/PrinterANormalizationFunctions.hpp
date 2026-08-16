#pragma once

#include "RIMValue.hpp"

namespace rim
{

RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context);

// IdentityNormalize と違い、生値(常に kDouble)を宣言された型(kBool/kInt32)へ
// 変換してから返す。素通しのままだと DataStoreDispatcher/ValueStore に積まれる
// RIMValue::type が宣言型とずれ、対応する RIM_GetBool/RIM_GetInt32 が
// 読み出せなくなるため、id ごとの目標型が既知の DataItem はこちらを使う。
RIMValue IdentityNormalizeBool(
    const RIMValue& value,
    const void* context);

RIMValue IdentityNormalizeInt32(
    const RIMValue& value,
    const void* context);

// context は `const SourceUnit*` として解釈する(SourceUnit.hpp)。
// nullptr(未指定)は SourceUnit::kNormalized と同義(換算しない)。
RIMValue NormalizeTemperatureSensorA(
    const RIMValue& value,
    const void* context);

RIMValue NormalizeBinary(
    const RIMValue& value,
    const void* context);


} // namespace rim