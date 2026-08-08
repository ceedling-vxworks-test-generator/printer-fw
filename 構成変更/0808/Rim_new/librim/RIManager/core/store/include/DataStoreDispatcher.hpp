#pragma once

#include <atomic>
#include <thread>

#include "CapabilityInputQueue.hpp"
#include "IRIMSnapshotReader.hpp"
#include "ProductDefinition.hpp"

namespace rim
{

class DataStoreQueue;
class ValueStore;
class IChangeChecker;

class DataStoreDispatcher
{
public:

    DataStoreDispatcher(
        const ProductDefinition& product,
        DataStoreQueue& queue,
        ValueStore& store,
        const IChangeChecker& changeChecker,
        IRIMSnapshotReader& reader,
        CapabilityInputQueue& capabilityQueue,
        uint32_t priority);

    ~DataStoreDispatcher();
    void start();
    void stop();
    bool ExecuteOnce();

private:

    void run();
    void ProcessItem(RIMDataItem& item);

private:

    const ProductDefinition& product_;
    DataStoreQueue& queue_;
    ValueStore& store_;
    const IChangeChecker& changeChecker_;
    CapabilityInputQueue& capabilityQueue_;
    uint32_t priority_;
    std::atomic<bool> running_{false};
    std::thread workerThread_;

    IRIMSnapshotReader& reader_; // いずれ消す
};

} // namespace rim