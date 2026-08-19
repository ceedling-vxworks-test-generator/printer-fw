#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <thread>

#include "StoreInputQueue.hpp"
#include "DomainStorageRegistry.hpp"
#include "IRIMSnapshotReader.hpp"

#include "RouteProvider.hpp"
#include "IRIMStoreUpdateNotifier.hpp"
#include "DataStoreDispatcher.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class DataStoreDispatcher;

class DataStoreWorker
{
public:

    DataStoreWorker(
        const ProductDefinition& product,
        StoreInputQueue& queue,
        DomainStorageRegistry& domainStore,
        IRIMSnapshotReader& reader,
        RouteProvider& routeProvider)
        : product_(product)
        , queue_(queue)
        , domainStore_(domainStore)
        , reader_(reader)
        , routeProvider_(routeProvider)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    const ProductDefinition& product_;

    StoreInputQueue& queue_;

    DomainStorageRegistry&
        domainStore_;

    IRIMStoreUpdateNotifier* notifier_ {};

    IRIMSnapshotReader& reader_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
    RouteProvider& routeProvider_;
};

} // namespace rim