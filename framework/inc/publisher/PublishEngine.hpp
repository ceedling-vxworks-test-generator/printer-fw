#pragma once

//
// PublishEngine - RIM_PublisherLayer(L4)の中核(仕様 §7.1)。IPublisher を実装。
// notify を受け、トリガ評価・Rate Limit・関心Cap一致(Broker)・配信的差分(StateRepository)を経て
// 購読者へ Push 配信する。Event / 優先度Critical・High は Rate Limit 除外(仕様 §8)。
//

#include <chrono>
#include <cstdint>

#include "publisher/IPublisher.hpp"
#include "publisher/ISubscriber.hpp"
#include "publisher/SubscriptionSet.hpp"
#include "publisher/SubscriptionBroker.hpp"
#include "publisher/StateRepository.hpp"
#include "datastore/RIMResult.hpp"

namespace rim
{

class PublishEngine final
    : public IPublisher
{
public:

    void Init();
    void Shutdown();

    // 購読登録。既に配信済み状態があれば Initial として初回配信する。
    RIMResult Subscribe(ISubscriber* sub, const SubscriptionSet& set = SubscriptionSet{});
    void      Unsubscribe(ISubscriber* sub);

    // Rate Limit の最小配信間隔(ms)。0 で無効(既定)。
    void SetMinIntervalMs(std::uint32_t ms) { minIntervalMs_ = ms; }

    // IPublisher: 生成済Capabilityを受け配信処理を起動する。
    void Notify(const MachineCapability& capability) override;

private:

    bool RateLimited(const MachineCapability& mc);

    SubscriptionBroker broker_;
    StateRepository    stateRepo_;

    std::uint32_t minIntervalMs_{0};
    std::chrono::steady_clock::time_point lastPublish_{};
    bool          lastPublishValid_{false};

    MachineCapability last_{};
    bool              hasLast_{false};
};

} // namespace rim
