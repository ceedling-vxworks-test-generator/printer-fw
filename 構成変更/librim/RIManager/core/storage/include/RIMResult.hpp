#pragma once

//
// RIMResult - 共通結果コード。
//

namespace rim
{

enum class RIMResult
{
    kOk = 0,
    kErrConvert,
    kErrClassify,
    kErrPost,          // Queue/Buffer 投入失敗(満杯=喪失)
    kErrKindMismatch,  // post種別とId分類の不一致、value型とidの不一致
    kErrInvalidArg
};

} // namespace rim
