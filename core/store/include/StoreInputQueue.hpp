#pragma once

#include "FixedQueue.hpp"
#include "RIMConfig.hpp"
#include "RIMDataItem.hpp"

namespace rim
{

//
// Adapter 段から Datastore 段へデータ項目を渡すキュー。
//
// 以前は std::queue + mutex + condition_variable を手書きしており、
// **push のたびにノードを動的確保**していた(定常運転中にヒープを触る)。
// 固定容量リング(FixedQueue)へ置き換えて確保をゼロにしてある。
//
// 満杯時は最も古いものを捨てて最新を残す。捨てたことは DroppedCount() で
// 観測できるので、消費が追いついていないことを検知できる。
//
using StoreInputQueue = FixedQueue<RIMDataItem, kStoreQueueDepth>;

} // namespace rim
