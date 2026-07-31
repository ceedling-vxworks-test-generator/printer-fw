#pragma once

#include <atomic>
#include <thread>

#include "CapabilityChangeTracker.hpp"
#include "CapabilityEvaluator.hpp"
#include "CapabilityInputQueue.hpp"
#include "CapabilityStore.hpp"

#include "PublisherInputQueue.hpp"

namespace rim
{

//
// CapabilityWorker - Snapshot を受けて Capability を作り直し、変化分を配信段へ渡す。
//
// 旧実装は CapabilityManager(5種の具象 Rule を直接持つ)と
// MachineCapabilityStore(5種の具象 Capability を直接持つ)に依存していたため、
// Capability を1つ増やすたびに Core の改修が必要だった。
// 本実装が触るのは CapabilityEvaluator / CapabilityStore / CapabilityChangeTracker
// だけで、いずれも CapabilityId と型消去済みバイト列しか扱わない。
//
class CapabilityWorker
{
public:

    CapabilityWorker(
        CapabilityInputQueue& queue,
        CapabilityEvaluator& evaluator,
        CapabilityStore& store,
        PublisherInputQueue& publisherQueue)
        : queue_(queue)
        , evaluator_(evaluator)
        , store_(store)
        , publisherQueue_(publisherQueue)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

    // 差分判定の既定はバイト比較。誤差許容などが要る Capability は
    // Product が ICapabilityDiffRule を実装してここへ登録する。
    bool SetDiffRule(
        CapabilityId id,
        const ICapabilityDiffRule* rule)
    {
        return changeTracker_.SetDiffRule(
            id,
            rule);
    }

private:

    // Snapshot 1件を処理して、変化があれば配信段へ積む。
    void ProcessSnapshot(
        const RIMSnapshot& snapshot);

    CapabilityInputQueue& queue_;

    CapabilityEvaluator& evaluator_;

    CapabilityStore& store_;

    CapabilityChangeTracker
        changeTracker_;

    PublisherInputQueue& publisherQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim
