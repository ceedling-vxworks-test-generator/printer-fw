#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "FaultApplier.hpp"
#include "FaultSnapshotQueue.hpp"
#include "StoreInputQueue.hpp"
#include "ValueStore.hpp"
#include "IRIMSnapshotReader.hpp"

#include "CapabilityInputQueue.hpp"

namespace rim
{

//
// DataStoreWorker - L2。受理した項目を格納し、Capability 段へ Snapshot を送る。
//
// 入力の性質(現在値か異常報告か)を判別するのはこの層の責務である
// (Adapter は正規化して渡すだけ)。判別材料は DataContext::faultState で、
// 値が入っていれば ErrorRepository へ、無ければ ValueStore へ入れる。
//
// **レーンは1本のまま**であり、性質ごとにキューは分けていない。
// 分けているのは格納先だけである。
//
class DataStoreWorker
{
public:

    // faultApplier は省略可(nullptr なら異常報告は無視して値として扱わない)。
    // faultSnapshotQueue も省略可(nullptr なら FaultInfoList 経路自体を持たない)。
    // 同じスレッドで両方のキューを見ることで、ErrorRepository への書き込みは
    // 単一スレッドのままにしてある(併存させるためにスレッドを増やさない)。
    DataStoreWorker(
        StoreInputQueue& queue,
        ValueStore& store,
        IRIMSnapshotReader& reader,
        CapabilityInputQueue& capabilityQueue,
        FaultApplier* faultApplier = nullptr,
        FaultSnapshotQueue* faultSnapshotQueue = nullptr)
        : queue_(queue)
        , store_(store)
        , reader_(reader)
        , capabilityQueue_(capabilityQueue)
        , faultApplier_(faultApplier)
        , faultSnapshotQueue_(faultSnapshotQueue)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    // 項目1件を処理し、Capability 段へ Snapshot を送る。
    void ProcessItem(
        const RIMDataItem& item);

    // FaultInfoList 1回分(スナップショット全件)を処理し、Capability 段へ Snapshot を送る。
    void ProcessFaultSnapshot(
        const FaultSnapshotBatch& batch);

    StoreInputQueue& queue_;

    ValueStore& store_;

    IRIMSnapshotReader& reader_;

    CapabilityInputQueue& capabilityQueue_;

    FaultApplier* faultApplier_;

    FaultSnapshotQueue* faultSnapshotQueue_;

    // queue_ の WaitAndPopFor に使う待ち時間。faultSnapshotQueue_ も同じスレッドで
    // 見るため、無期限には待たない(この間隔だけスナップショット検知が遅れうる)。
    static constexpr std::chrono::milliseconds kPollInterval{10};

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim