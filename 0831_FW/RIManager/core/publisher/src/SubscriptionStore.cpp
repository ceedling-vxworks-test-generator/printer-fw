#include "SubscriptionStore.hpp"

namespace rim
{

SubscriptionId
SubscriptionStore::CreateSubscriptionId()
{
    // TODO: ログ追加候補
    // SubscriptionId採番時に生成値を出力すると
    // 購読追跡に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);
    return nextId_++;
}

void SubscriptionStore::Register(
    const SubscriptionInfo& info)
{
    // TODO: ログ追加候補
    // 購読登録時に SubscriptionId、Target、Trigger、Method を
    // 出力すると購読設定確認に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);
    subscriptions_.push_back(info);
}

bool SubscriptionStore::Find(
    SubscriptionId id,
    SubscriptionInfo& out) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& info : subscriptions_)
    {
        if (info.id == id)
        {
            out = info;
            return true;
        }
    }

    // TODO: ログ追加候補
    // 購読情報取得失敗時に SubscriptionId を出力すると
    // 設定不整合の調査に利用できる。

    return false;
}

void SubscriptionStore::GetSubscriptions(
    const RIMId& target,
    NotificationTrigger trigger,
    std::vector<SubscriptionInfo>& subscriptions) const
{
    // TODO: ログ追加候補
    // TargetとTriggerに一致した購読件数を出力すると
    // 通知対象選定結果の確認に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);
    subscriptions.clear();

    for (const auto& info : subscriptions_)
    {
        if (info.target.type != target.type)
        {
            continue;
        }

        if (info.target.id != target.id)
        {
            continue;
        }

        if (info.trigger != trigger)
        {
            continue;
        }

        subscriptions.push_back(info);

        // TODO: ログ追加候補
        // TargetType、TargetId、Trigger、
        // 抽出件数を出力すると通知対象確認に利用できる。
    }
}

} // namespace rim