#pragma once

#include "RIMValue.hpp"

namespace rim
{

// 要求定義書のNormalization列は全項目「-」(正規化不要)のため、
// 素通しのIdentityNormalizeのみを用意する。
RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context);

}
