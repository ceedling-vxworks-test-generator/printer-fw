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
                    RIMDataItem item{};

                    if (!queue_.WaitAndPop(
                            item))
                    {
                        break;
                    }

                    ProcessItem(
                        item);
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