#pragma once

//
// Publisher - L4 RIM_PublisherLayer(C++ OO 移植)。
// rim_publisher.cpp の移植。購読者を固定長配列で保持し、Capability を Push 配信する。
//

#include <cstddef>

#include "capability/CapabilitySet.hpp"
#include "datastore/RIMResult.hpp"
#include "publisher/ISubscriber.hpp"

namespace rim
{

class Publisher
{
public:

    static constexpr std::size_t kMaxSubscribers = 8;

    void Init();
    void Shutdown();

    // 購読登録(満杯なら kErrFull)。
    RIMResult Subscribe(ISubscriber* subscriber);

    // 全購読者へ Push 配信(L3から呼ばれる)。
    void Publish(const CapabilitySet& cap);

private:

    ISubscriber* subscribers_[kMaxSubscribers]{};
    std::size_t  count_{0};
};

} // namespace rim
