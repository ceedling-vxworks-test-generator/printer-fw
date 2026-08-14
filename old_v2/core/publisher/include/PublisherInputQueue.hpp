#pragma once

#include "FixedQueue.hpp"
#include "RIMConfig.hpp"
#include "CapabilityChangeList.hpp"

namespace rim
{

//
// Capability 段から配信段へ「変化した id の一覧」を渡すキュー。
//
// 以前は std::queue + mutex + condition_variable を手書きしており、
// **push のたびにノードを動的確保**していた(定常運転中にヒープを触る)。
// 固定容量リング(FixedQueue)へ置き換えて確保をゼロにしてある。
//
// 満杯時は最も古いものを捨てて最新を残す。捨てたことは DroppedCount() で
// 観測できるので、消費が追いついていないことを検知できる。
//
using PublisherInputQueue = FixedQueue<CapabilityChangeList, kPublisherQueueDepth>;

} // namespace rim
