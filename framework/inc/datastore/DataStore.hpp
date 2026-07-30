#pragma once

//
// DataStore - RIM_DatastoreLayer(L2)ファサード。
// 仕様の全コンポーネント(CentralInputPort / 3レーン / 3Dispatcher / MachineRegistry /
// MachineSnapshotReader)を所有・結線する。Adapter は post、Capability は Reader を用いる。
// Dispatch() は Fault→Operation→CurrentValue の順に各Dispatcherを1回駆動する。
//

#include "datastore/InputQueues.hpp"
#include "datastore/CurrentValueBuffer.hpp"
#include "datastore/MachineRegistry.hpp"
#include "datastore/FaultDispatcher.hpp"
#include "datastore/OperationDispatcher.hpp"
#include "datastore/CurrentValueDispatcher.hpp"
#include "datastore/CentralInputPort.hpp"
#include "datastore/MachineSnapshotReader.hpp"
#include "datastore/IInputClassifier.hpp"
#include "datastore/IRegistryUpdateNotifier.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/Snapshot.hpp"

namespace rim
{

class DataStore
{
public:

    explicit DataStore(const IInputClassifier& classifier)
        : faultDispatcher_(faultQueue_, registry_.Fault())
        , operationDispatcher_(operationQueue_, registry_.Operation())
        , currentDispatcher_(currentBuffer_, registry_.CurrentValue())
        , port_(faultQueue_, operationQueue_, currentBuffer_, classifier)
        , reader_(registry_)
    {
    }

    // 更新通知先を各Dispatcherへ結線。
    void Init(IRegistryUpdateNotifier* notifier)
    {
        faultDispatcher_.SetNotifier(notifier);
        operationDispatcher_.SetNotifier(notifier);
        currentDispatcher_.SetNotifier(notifier);
    }

    void Shutdown()
    {
        faultDispatcher_.SetNotifier(nullptr);
        operationDispatcher_.SetNotifier(nullptr);
        currentDispatcher_.SetNotifier(nullptr);
    }

    // --- CentralInputPort(Adapter向け) ---
    // Adapter は本IF(ICentralInputPort)にのみ依存する。受理点は Post() の1本で、
    // 性質判別とレーン振り分けは CentralInputPort(L2)が行う。
    ICentralInputPort& Port() { return port_; }

    RIMResult Post(const RIMDataItem& item) { return port_.Post(item); }

    // --- Dispatcher駆動(周期/イベント。スケルトンでは明示呼び出し) ---
    void Dispatch()
    {
        faultDispatcher_.Dispatch();
        operationDispatcher_.Dispatch();
        currentDispatcher_.Dispatch();
    }

    // --- Snapshot(Capability/Accessor向け) ---
    IMachineSnapshotReader& Reader() { return reader_; }
    MachineSnapshot Capture(const SnapshotRequest& req) const { return reader_.Capture(req); }

    MachineRegistry& Registry() { return registry_; }

private:

    FaultInputQueue        faultQueue_;
    OperationReportQueue   operationQueue_;
    CurrentValueBuffer     currentBuffer_;
    MachineRegistry        registry_;

    FaultDispatcher          faultDispatcher_;
    OperationDispatcher      operationDispatcher_;
    CurrentValueDispatcher   currentDispatcher_;
    CentralInputPort         port_;
    MachineSnapshotReader    reader_;
};

} // namespace rim
