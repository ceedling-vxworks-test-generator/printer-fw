#include "DataStoreWorker.hpp"

namespace rim
{

void DataStoreWorker::ProcessItem(
    const RIMDataItem& item)
{
    //
    // 異常報告かどうかで格納先を選ぶ(判別は L2 の責務)。
    // 異常は値ではないので ValueStore へは入れない。入れてしまうと
    // Snapshot に異常コードが「現在値」として混ざる。
    //
    if (faultApplier_ != nullptr &&
        FaultApplier::IsFault(item))
    {
        faultApplier_->Apply(
            item);
    }
    else
    {
        store_.Store(
            item);
    }

    // 異常でも値でも、状態が変わった可能性があるので Capability を作り直す。
    capabilityQueue_.Push(
        reader_.Read());
}

void DataStoreWorker::ProcessFaultSnapshot(
    const FaultSnapshotBatch& batch)
{
    if (faultApplier_ != nullptr)
    {
        faultApplier_->ApplySnapshot(
            batch.entries,
            batch.count);
    }

    // ProcessItem と同様、変化の有無によらず Capability を作り直す。
    capabilityQueue_.Push(
        reader_.Read());
}

void DataStoreWorker::Run()
{
    if (running_)
    {
        return;
    }

    running_ = true;

    workerThread_ =
        std::thread(
            [this]
            {
                while (running_)
                {
                    // FaultInfoList 経路は別レーンなので先に見る(併存させるために
                    // スレッドを増やさず、同じスレッドで両方さばく)。
                    if (faultSnapshotQueue_ != nullptr)
                    {
                        FaultSnapshotBatch batch{};

                        if (faultSnapshotQueue_->TryPop(
                                batch))
                        {
                            ProcessFaultSnapshot(
                                batch);

                            continue;
                        }
                    }

                    RIMDataItem item{};

                    // 無期限に待つと faultSnapshotQueue_ の検知が遅れるため、
                    // 一定間隔で戻ってきて両方を見直す。
                    if (queue_.WaitAndPopFor(
                            kPollInterval,
                            item))
                    {
                        ProcessItem(
                            item);
                    }
                }
            });
}

void DataStoreWorker::Stop()
{
    running_ = false;

    queue_.Shutdown();

    if (workerThread_.joinable())
    {
        workerThread_.join();
    }
}

bool DataStoreWorker::ExecuteOnce()
{
    if (faultSnapshotQueue_ != nullptr)
    {
        FaultSnapshotBatch batch{};

        if (faultSnapshotQueue_->TryPop(
                batch))
        {
            ProcessFaultSnapshot(
                batch);

            return true;
        }
    }

    RIMDataItem item{};

    if (!queue_.TryPop(
            item))
    {
        return false;
    }

    ProcessItem(
        item);

    return true;
}

} // namespace rim