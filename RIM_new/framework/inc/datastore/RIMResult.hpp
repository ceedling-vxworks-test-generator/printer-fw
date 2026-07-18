#pragma once

//
// RIMResult - 共通結果コード。rim_result_t 相当。
//

namespace rim
{

enum class RIMResult
{
    kOk = 0,
    kErrRuleNotFound,
    kErrConvert,
    kErrClassify,
    kErrPost,          // Queue/Buffer 投入失敗(満杯)
    kErrFull,          // 固定容量コンテナ満杯
    kErrKindMismatch,  // post種別とId分類の不一致
    kErrInvalidArg
};

} // namespace rim
