#pragma once

//
// PublishTrigger - 配信を起動する契機(仕様 §8)。
//   OnChange  : 配信的差分あり(State Repository)
//   Periodic  : 一定周期(Publish Engine)
//   Threshold : 連続値の閾値跨ぎ(判定はCapability、通知を受けて配信)
//   Event     : 明示イベント(Error発生/Job完了等)。Rate Limit除外
//   Initial   : 購読開始時の初回配信(Subscription Broker)
//

namespace rim
{

enum class PublishTrigger
{
    kOnChange = 0,
    kPeriodic,
    kThreshold,
    kEvent,
    kInitial
};

} // namespace rim
