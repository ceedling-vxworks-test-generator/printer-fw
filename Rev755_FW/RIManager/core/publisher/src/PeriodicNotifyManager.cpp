#include "PeriodicNotifyManager.hpp"

#include <chrono>

namespace rim
{

void PeriodicNotifyManager::Register(
    const PeriodicCondition& condition)
{
    // TODO: ログ追加候補
    // 定期通知登録時に SubscriptionId、Interval を出力すると
    // 定期通知設定内容の確認に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);
    conditions_[condition.subscriptionId] = condition;
}

void PeriodicNotifyManager::GetDueConditions(
    std::vector<PeriodicCondition>& out)
    const
{
    // TODO: ログ追加候補
    // Due対象取得時に取得件数を出力すると
    // 定期通知実行状況の確認に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);

    out.clear();

    const auto now = std::chrono::steady_clock::now();

    for (const auto& pair : conditions_)
    {
        const auto& condition = pair.second;

        if (condition.nextNotifyTime <= now)
        {
            out.push_back( condition);
        }
    }
}

void PeriodicNotifyManager::UpdateNextTime(
    SubscriptionId subscriptionId)
{
    // TODO: ログ追加候補
    // 次回通知時刻更新時に SubscriptionId と
    // 更新後時刻を出力すると定期通知遅延の調査に利用できる。

    std::lock_guard<std::mutex> lock(mutex_);

    auto it = conditions_.find( subscriptionId);

    if (it == conditions_.end())
    {
        return;
    }

    it->second.nextNotifyTime +=
        it->second.interval;
}

} // namespace rim