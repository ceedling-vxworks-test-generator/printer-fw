#pragma once

#include <atomic>
#include <thread>

#include "datastore/StoreInputQueue.hpp"
#include "datastore/ValueStore.hpp"
#include "datastore/AggregateRIMSnapshotReader.hpp"

#include "capability/CapabilityInputQueue.hpp"

namespace rim
{

class DataStoreWorker
{
public:

    DataStoreWorker(
        StoreInputQueue& queue,
        ValueStore& store,
        AggregateRIMSnapshotReader& reader,
        CapabilityInputQueue& capabilityQueue)
        : queue_(queue)
        , store_(store)
        , reader_(reader)
        , capabilityQueue_(capabilityQueue)
    {
    }

    void Run();

    void Stop();

    bool ExecuteOnce();

private:

    StoreInputQueue& queue_;

    ValueStore& store_;

    AggregateRIMSnapshotReader& reader_;

    CapabilityInputQueue& capabilityQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim