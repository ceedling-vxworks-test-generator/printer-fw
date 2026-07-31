#pragma once

//
// RIMDataId - 新機種を起こすときの雛形。
//
// products/<機種>/DataItem/ へコピーし、その機種が扱うデータ種別へ差し替えること。
// core はヘッダ名だけを見ており、列挙子の中身は知らない。
//
// 守ること:
//   - 0 から連番にする(添字として使われる)
//   - 末尾に kCount 番兵を置く
//   - core/products のどちらからも参照されるので、名前を機種間で揃える必要は無い
//

namespace rim
{

enum class RIMDataId
{
    kSampleValue = 0,

    kCount
};

inline constexpr int ToIndex(RIMDataId id)
{
    return static_cast<int>(id);
}

inline constexpr int kRIMDataIdCount =
    static_cast<int>(RIMDataId::kCount);

} // namespace rim
