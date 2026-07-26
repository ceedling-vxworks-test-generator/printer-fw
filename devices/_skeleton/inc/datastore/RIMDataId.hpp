#pragma once

//
// RIMDataId - 新機種テンプレ。データ種別識別子(連番・固定長配列前提)。機種固有(devices側)。
// TODO: この機種が持つデータ種別を列挙する。kCount は最後に置く番兵(削除しないこと)。
//
// framework は "datastore/RIMDataId.hpp" というヘッダ名(RIMDataId型・ToIndex()・
// kRIMDataIdCount)にのみ依存し、この列挙子の中身を一切知らない。新機種を追加する際は
// このファイルを devices/<機種名>/inc/datastore/RIMDataId.hpp としてコピーし、
// TODO 部分を実データに置き換える(CMakeLists.txt のインクルードパス切替だけで
// framework 側は無改修のまま差し替わる)。
//

namespace rim
{

enum class RIMDataId
{
    // TODO: 例: kTemperature = 0, kHumidity, ...
    kCount = 0  // TODO: 実データを足したら kCount は末尾に移す(番兵)。
};

inline constexpr int ToIndex(RIMDataId id) { return static_cast<int>(id); }
inline constexpr int kRIMDataIdCount = static_cast<int>(RIMDataId::kCount);

} // namespace rim
