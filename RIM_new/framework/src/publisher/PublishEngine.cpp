#include "publisher/PublishEngine.hpp"

namespace rim
{

void PublishEngine::Init()
{
    broker_.Clear();
    stateRepo_.Clear();
    lastPublishValid_ = false;
    hasLast_          = false;
}

void PublishEngine::Shutdown()
{
    broker_.Clear();
    stateRepo_.Clear();
}

RIMResult PublishEngine::Subscribe(ISubscriber* sub, const SubscriptionSet& set)
{
    RIMResult r = broker_.Subscribe(sub, set);
    if (r != RIMResult::kOk) return r;

    // Initial: 既に配信済み状態があれば、この購読者へ初回配信する。
    if (hasLast_ && sub != nullptr) {
        MachineCapability initial = last_;
        initial.trigger = PublishTrigger::kInitial;
        sub->OnPublish(initial);
        stateRepo_.Update(sub, initial.capabilities);
    }
    return RIMResult::kOk;
}

void PublishEngine::Unsubscribe(ISubscriber* sub)
{
    broker_.Unsubscribe(sub);
    stateRepo_.Forget(sub);
}

bool PublishEngine::RateLimited(const MachineCapability& mc)
{
    // Event / Critical / High は間引かない。
    if (mc.trigger == PublishTrigger::kEvent) return false;
    if (mc.priority == CapabilityPriority::kCritical ||
        mc.priority == CapabilityPriority::kHigh) return false;
    if (minIntervalMs_ == 0) return false;

    const auto now = std::chrono::steady_clock::now();
    if (lastPublishValid_) {
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPublish_).count();
        if (elapsed < static_cast<long long>(minIntervalMs_)) return true;  // 抑制
    }
    return false;
}

void PublishEngine::Notify(const MachineCapability& mc)
{
    // 最新状態は常に保持(Initial配信/トレーリングエッジの基礎)。
    last_    = mc;
    hasLast_ = true;

    if (RateLimited(mc)) {
        // TODO: トレーリングエッジ配信(間隔満了後に最新を送出)は周期タスク導入時に実装。
        return;
    }

    for (std::size_t i = 0; i < broker_.Count(); ++i) {
        ISubscriber* sub = broker_.SubscriberAt(i);
        const SubscriptionSet& set = broker_.SubscriptionAt(i);

        // 関心Cap一致判定(Subscription Broker)。
        if (!set.IsInterested(mc.changedKinds)) continue;

        // 配信的差分判定(State Repository)。Event は常に配信。
        const bool needDelivery =
            (mc.trigger == PublishTrigger::kEvent) ||
            stateRepo_.HasChangedFor(sub, mc.capabilities);
        if (!needDelivery) continue;

        sub->OnPublish(mc);
        stateRepo_.Update(sub, mc.capabilities);
    }

    lastPublish_      = std::chrono::steady_clock::now();
    lastPublishValid_ = true;
}

} // namespace rim
