#pragma once

//
// DataStore - L2 RIM_DatastoreLayer(C++ OO 移植)。
// rim_datastore.cpp の性質別ステージングを移植:
//   - FAULT     : RingBuffer FIFO ＋ FixedMap レジストリ(add/remove/update/clear_all)
//   - OPERATION : RingBuffer FIFO ＋ 最新JobProgress
//   - CURRENT   : slot＋dirty の latest-wins
// 排他(H-7)は std::mutex で直列化。Dispatch は通知発火の前にロック解放する
// (CapabilityManager の通知先が Capture を再入呼びするため)。
//

#include <cstdint>
#include <mutex>

#include "datastore/Config.hpp"
#include "datastore/RIMDataId.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RIMResult.hpp"
#include "datastore/DomainSet.hpp"
#include "datastore/Snapshot.hpp"
#include "datastore/IUpdateNotifier.hpp"

#include "adapter/IDataProfile.hpp"

#include "container/RingBuffer.hpp"
#include "container/FixedMap.hpp"

namespace rim
{

class DataStore
{
public:

    explicit DataStore(const IDataProfile& profile)
        : profile_(profile)
    {
    }

    // 更新通知先を登録して初期化。notifier は nullptr 可(通知無効)。
    RIMResult Init(IUpdateNotifier* notifier);
    void      Shutdown();

    // CentralInputPort: 性質別3種post。
    RIMResult PostFault(const RIMDataItem& item);
    RIMResult PostOperation(const RIMDataItem& item);
    RIMResult PostCurrent(const RIMDataItem& item);

    // 各レーンの取り込み→Registry反映→変化があれば OnUpdated を発火。
    void Dispatch();

    // MachineSnapshotReader。
    RIMResult Capture(const SnapshotRequest& req, MachineSnapshot& out) const;

private:

    struct FaultEntry
    {
        std::uint8_t active{1};  // 1=active, 0=healed
    };

    struct FaultEvent
    {
        CollectionOp op{CollectionOp::kAdd};
        std::uint32_t key{0};
        FaultEntry    payload{};
    };

    struct OpEvent
    {
        std::uint8_t jobProgress{0};
    };

    struct CurrentSlot
    {
        RIMValue value{};
        bool     present{false};
        bool     dirty{false};
    };

    void Fire(DomainSet domains);

    const IDataProfile& profile_;
    IUpdateNotifier*    notifier_{nullptr};

    mutable std::mutex mutex_;

    CurrentSlot current_[kRIMDataIdCount];

    RingBuffer<FaultEvent, kFaultQueueDepth>          faultRing_;
    FixedMap<std::uint32_t, FaultEntry, kFaultCap>    faultMap_;

    RingBuffer<OpEvent, kOpQueueDepth> opRing_;
    bool          opPresent_{false};
    std::uint8_t  opJobProgress_{0};
};

} // namespace rim
