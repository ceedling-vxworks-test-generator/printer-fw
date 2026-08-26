#include "RouteExecutor.hpp"

#include "DataStoreWorker.hpp"
#include "CapabilityWorker.hpp"

#include "ICapabilitySnapshotProvider.hpp"
#include "SnapshotAccessor.hpp"

#include "PartitionStorageRegistry.hpp"
#include "RouteProvider.hpp"

namespace rim
{

RouteExecutor::RouteExecutor(
    const ProductContext& context,
    const RouteQueues& queues,
    PartitionStorageRegistry& domainStore,
    CapabilityManager& capabilityManager,
    FacadeManager& facadeManager,
    PublisherInputQueue& publisherQueue,
    const ICapabilitySnapshotProvider& capabilitySnapshotProvider,
    const IFacadeSnapshotProvider& facadeSnapshotProvider)
{
    dataStoreWorker_ =
        std::make_unique<DataStoreWorker>(
            context,
            *queues.storeQueue,
            domainStore,
            *queues.capabilityQueue);

    capabilityWorker_ =
        std::make_unique<CapabilityWorker>(
            *queues.capabilityQueue,
            capabilityManager,
            facadeManager,
            publisherQueue,
            capabilitySnapshotProvider,
            facadeSnapshotProvider,
            context);
}

RouteExecutor::~RouteExecutor()
{
    Stop();
}

void
RouteExecutor::Start()
{

    // TODO: ログ追加候補
    // RouteExecutor開始時に対象Route名を出力すると
    // Worker起動状況の確認に利用できる。

    dataStoreWorker_->start();
    capabilityWorker_->start();
}

void
RouteExecutor::Stop()
{
    // TODO: ログ追加候補
    // RouteExecutor停止時に対象Route名を出力すると
    // 終了シーケンス確認に利用できる。
    if (stopped_)
    {
        return;
    }

    stopped_ = true;

    dataStoreWorker_->stop();
    capabilityWorker_->stop();
}

bool
RouteExecutor::ExecuteOnce()
{
    if (!dataStoreWorker_->ExecuteOnce())
    {

        // TODO: ログ追加候補
        // DataStoreWorkerに処理対象が無かった場合の発生頻度を
        // 記録すると負荷状況の確認に利用できる。

        return false;
    }

    if (!capabilityWorker_->ExecuteOnce())
    {
        
        // TODO: ログ追加候補
        // CapabilityWorkerに処理対象が無かった場合の発生頻度を
        // 記録すると処理経路の確認に利用できる。

        return false;
    }

    return true;
}

} // namespace rim