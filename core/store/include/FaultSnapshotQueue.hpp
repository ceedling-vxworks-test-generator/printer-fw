#pragma once

#include "FaultSnapshotEntry.hpp"
#include "FixedQueue.hpp"
#include "RIMConfig.hpp"

namespace rim
{

//
// FaultSnapshotBatch - FaultInfoList 1回分(「今存在する異常の全件」)。
//
// 動的確保をしないため、最大件数は ErrorRepository と同じ kMaxErrors に揃えてある
// (これを超えるスナップショットはそもそも ErrorRepository へ全件は入り得ない)。
//
struct FaultSnapshotBatch
{
    FaultSnapshotEntry entries[kMaxErrors]{};

    std::size_t count{0};
};

//
// FaultSnapshotQueue - Adapter 段から Datastore 段へスナップショットを渡すキュー。
//
// 単発の Push(StoreInputQueue、1件ずつのイベント通知)とは**別レーン**である。
// FaultInfoList は既存の Push と併存する新しい経路として設計されており
// (置き換えではない)、RIMValue がスカラのみで構造体一覧を運べないことから
// 専用のキューを用いる。反映は DataStoreWorker の同じスレッドが行うため、
// ErrorRepository への書き込みは従来通り単一スレッドのままである。
//
using FaultSnapshotQueue = FixedQueue<FaultSnapshotBatch, kFaultSnapshotQueueDepth>;

} // namespace rim
