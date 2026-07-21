#pragma once

//
// 性質別入力レーンの型(仕様 §6.2/§6.3)。喪失不可・順序保証のFIFO。
// CurrentValueBuffer は別クラス(§6.4)。
//

#include "datastore/Config.hpp"
#include "datastore/RIMDataItem.hpp"
#include "container/Queue.hpp"

namespace rim
{

using FaultInputQueue      = Queue<RIMDataItem, kFaultQueueDepth>;
using OperationReportQueue = Queue<RIMDataItem, kOpQueueDepth>;

} // namespace rim
