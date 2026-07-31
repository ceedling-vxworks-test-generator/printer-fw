#pragma once

namespace rim
{

// DataItem の所属ドメイン。DataDomain の定義はこのヘッダが唯一の正とする
// (以前 RIMDataDefinition.hpp にも列挙子の異なる別定義があり、両方を include した
//  翻訳単位がコンパイル不能になっていたため統合した)。
enum class DataDomain
{
    kDevice,
    kJob,
    kError,
    kConsumable,
    kMaintenance,
    kNetwork
};

} // namespace rim
