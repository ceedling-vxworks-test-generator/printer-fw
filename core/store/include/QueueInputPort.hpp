#pragma once

//
// QueueInputPort - ICentralInputPort の実装。Adapter が正規化した項目を
// Datastore 段のキューへ積む。
//
// Adapter は ICentralInputPort という抽象にしか依存しないため、
// 単体試験ではこのクラスの代わりにモックを差し込める。
//
// 入力性質(異常 / 稼働報告 / 現在値)の判別とレーン振り分けは L2 の責務であり、
// Adapter 側では行わない。現状は単一レーン(StoreInputQueue)へ集約している。
//

#include "ICentralInputPort.hpp"
#include "StoreInputQueue.hpp"

namespace rim
{

class QueueInputPort final
    : public ICentralInputPort
{
public:

    explicit QueueInputPort(
        StoreInputQueue& queue)
        : queue_(queue)
    {
    }

    RIMResult Post(
        const RIMDataItem& item) override
    {
        queue_.Push(
            item);

        return RIMResult::kOk;
    }

private:

    StoreInputQueue& queue_;
};

} // namespace rim
