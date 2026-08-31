#include "DeliveryHandler/CustomDeliveryHandler.hpp"

namespace rim
{

bool CustomDeliveryHandler::Deliver(
    const SubscriptionInfo& info,
    const NotificationMessage& message)
{
    // 製品固有の通知配送を実装するための関数。
    //
    // SubscriptionInfo と NotificationMessage の内容を利用して、
    // 通知先や通知形式を製品ごとに切り替えることを想定している。
    // 関連テスト：CustomDeliveryCanForwardNotificationToEventSender
    // 例:
    // ・SubscriptionInfo に応じて通知先を決定し、
    // ・NotificationMessage をイベントやメッセージへ変換して
    // ・外部システムへ送信する。
    //
    // 現在の  では独自配送要件が無いため、
    // 何も行わず成功として扱う。

    (void)info;
    (void)message;

    return true;
}

} // namespace rim
