#pragma once

#include <atomic>
#include <thread>

#include "FaultApplier.hpp"
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
    DataStoreWorker(
        StoreInputQueue& queue,
        ValueStore& store,
        IRIMSnapshotReader& reader,
        CapabilityInputQueue& capabilityQueue,
        FaultApplier* faultApplier = nullptr)
        : queue_(queue)
        , store_(store)
        , reader_(reader)
        , capabilityQueue_(capabilityQueue)
        , faultApplier_(faultApplier)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    // 項目1件を処理し、Capability 段へ Snapshot を送る。
    void ProcessItem(
        const RIMDataItem& item);

    StoreInputQueue& queue_;

    ValueStore& store_;

    IRIMSnapshotReader& reader_;

    CapabilityInputQueue& capabilityQueue_;

    FaultApplier* faultApplier_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim