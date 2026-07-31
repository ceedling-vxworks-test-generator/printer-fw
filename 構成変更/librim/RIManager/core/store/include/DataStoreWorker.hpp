#pragma once

#include <atomic>
#include <thread>

#include "StoreInputQueue.hpp"
#include "ValueStore.hpp"
#include "IRIMSnapshotReader.hpp"

#include "CapabilityInputQueue.hpp"

namespace rim
{

class DataStoreWorker
{
public:

    DataStoreWorker(
        StoreInputQueue& queue,
        ValueStore& store,
        IRIMSnapshotReader& reader,
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

    IRIMSnapshotReader& reader_;

    CapabilityInputQueue& capabilityQueue_;

    std::atomic<bool>
        running_{false};

    std::thread
        workerThread_;
};

} // namespace rim