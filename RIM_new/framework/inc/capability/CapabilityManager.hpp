#pragma once

//
// CapabilityManager - L3 RIM_CapabilityLayer(C++ OO 移植)。
// rim_capability.cpp の移植。IUpdateNotifier を実装し、L2 の更新通知を受けて
// Snapshot を取得 → 機種の Evaluator 集合で Capability を生成 → L4 Publisher へ配信。
// 機種可変の判定は ICapabilityEvaluatorSet(devices) を注入して差し込む。
//

#include "capability/CapabilitySet.hpp"
#include "capability/ICapabilityEvaluatorSet.hpp"

#include "datastore/IUpdateNotifier.hpp"
#include "datastore/DataStore.hpp"

#include "publisher/Publisher.hpp"

namespace rim
{

class CapabilityManager
    : public IUpdateNotifier
{
public:

    CapabilityManager(
        DataStore& store,
        const ICapabilityEvaluatorSet& evaluators,
        Publisher& publisher)
        : store_(store)
        , evaluators_(evaluators)
        , publisher_(publisher)
    {
    }

    void Init();
    void Shutdown();

    // L2 からの更新通知(IUpdateNotifier)。
    void OnUpdated(DomainSet domains) override;

    // 直近に生成した Capability をコピー取得(Accessor の Pull で利用)。
    bool Current(CapabilitySet& out) const;

private:

    DataStore&                     store_;
    const ICapabilityEvaluatorSet& evaluators_;
    Publisher&                     publisher_;

    CapabilitySet current_{};
    bool          hasCurrent_{false};
};

} // namespace rim
