#pragma once

#include <queue>
#include <mutex>

#include "NotificationMessage.hpp"

namespace rim
{

class SubscriberMailbox
{
public:

    explicit SubscriberMailbox(
    std::size_t maxSize = 1000)
    : maxSize_(maxSize)
    {
    }

    bool Push(
        const NotificationMessage& message)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (notificationQueue_.size() >= maxSize_)
        {
            ++overflowCount_;

            // TODO: ログ追加候補
            // Mailbox Overflow発生時に
            // Queueサイズと通知情報を出力する。

            return false;
        }

        notificationQueue_.push(message);

        // TODO: ログ追加候補
        // Queue投入時の件数を出力すると
        // 通知滞留状況の確認に利用できる。

        return true;
    }

    bool Pop(
        NotificationMessage& message)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (notificationQueue_.empty())
        {
            // TODO: ログ追加候補
            // 空Mailbox参照時の発生頻度を記録すると
            // ポーリング過多の調査に利用できる。

            return false;
        }

        message = notificationQueue_.front();

        notificationQueue_.pop();

        return true;
    }

    uint32_t Count() const
    {
        // TODO: ログ追加候補
        // Queue件数取得時の値を出力すると
        // 通知滞留状況の確認に利用できる。

        std::lock_guard<std::mutex> lock(mutex_);

        return static_cast<uint32_t>(
            notificationQueue_.size());
    }

    uint64_t GetOverflowCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return overflowCount_;
    }
    
private:

    std::size_t maxSize_;
    uint64_t overflowCount_{0};
    mutable std::mutex mutex_;
    std::queue<NotificationMessage> notificationQueue_;

};

} // namespace rim