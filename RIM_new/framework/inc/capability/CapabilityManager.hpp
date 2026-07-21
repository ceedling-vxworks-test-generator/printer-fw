#pragma once

//
// CapabilityManager - RIM_CapabilityLayer(L3)の中心(仕様 §8.4)。
// IRegistryUpdateNotifier を実装し、更新通知→Snapshot取得→Builder生成→DiffChecker→
// PriorityChecker→Publisher通知を制御する。ErrorCap/JobCap変化時は Event として通知。
// 機種可変の判定は ICapabilityBuilder(devices)を注入。
//

#include "capability/CapabilitySet.hpp"
#include "capability/MachineCapability.hpp"
#include "capability/ICapabilityBuilder.hpp"
#include "capability/CapabilityDiffChecker.hpp"
#include "capability/CapabilityPriorityChecker.hpp"

#include "datastore/IRegistryUpdateNotifier.hpp"
#include "datastore/IMachineSnapshotReader.hpp"

#include "publisher/IPublisher.hpp"

namespace rim
{

class CapabilityManager
    : public IRegistryUpdateNotifier
{
public:

    CapabilityManager(
        IMachineSnapshotReader& reader,
        const ICapabilityBuilder& builder,
        IPublisher& publisher)
        : reader_(reader), builder_(builder), publisher_(publisher)
    {
    }

    void Init();
    void Shutdown();

    // L2 からの更新通知(IRegistryUpdateNotifier)。
    void NotifyUpdated(RegistryDomainSet domains) override;

    // 直近生成した Capability を取得(Accessor の Pull)。
    bool Current(CapabilitySet& out) const;

private:

    IMachineSnapshotReader&    reader_;
    const ICapabilityBuilder&  builder_;
    IPublisher&                publisher_;

    CapabilityDiffChecker      diffChecker_;
    CapabilityPriorityChecker  priorityChecker_;

    CapabilitySet prev_{};
    bool          hasPrev_{false};
    CapabilitySet current_{};
    bool          hasCurrent_{false};
};

} // namespace rim
