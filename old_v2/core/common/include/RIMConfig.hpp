#pragma once

#include <cstddef>

namespace rim
{

//
// RIM 全体の容量パラメータ。
//
// 組込み向けに **定常運転中の動的確保をしない** ため、各コンテナの上限をここで
// 決め打ちにしている。実行時に伸びることはなく、上限を超えた分は取りこぼす
// (取りこぼしは各クラスが観測可能な形で返す)。
//
// 上限を変えるときは RAM 使用量に直結することに注意すること。
//

// Snapshot / ValueStore が扱えるデータ項目の最大数。
// **機種が定義する RIMDataId の総数以上** にすること。
// (足りないと、あるデータが Snapshot に載らず Capability が古い値のままになる)
inline constexpr std::size_t kMaxDataItems = 64;

// 同時に保持できる異常の最大数。
inline constexpr std::size_t kMaxErrors = 32;

// 各段のキューが保持できる件数。
// 生産側が消費側より速い場合、ここが埋まると **最も古いものから捨てる**。
// 捨てたことは各キューの DroppedCount() で観測できる。
inline constexpr std::size_t kStoreQueueDepth      = 64;
inline constexpr std::size_t kCapabilityQueueDepth = 16;
inline constexpr std::size_t kPublisherQueueDepth  = 16;

// FaultInfoList(スナップショット一括反映)を保持できる件数。
// 1件が kMaxErrors 件分の配列を丸ごと持つため、他のキューより浅くしてある。
inline constexpr std::size_t kFaultSnapshotQueueDepth = 4;

} // namespace rim
