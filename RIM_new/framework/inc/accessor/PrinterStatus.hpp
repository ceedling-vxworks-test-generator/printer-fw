#pragma once

//
// PrinterStatus - Accessor Layer のデータモデル(仕様 §8)。読み取り専用・取得時点整合。
//

#include <optional>

#include "datastore/Snapshot.hpp"
#include "datastore/RegistryDomain.hpp"
#include "capability/CapabilitySet.hpp"

namespace rim
{

struct PrinterStatusRequest
{
    RegistryDomainSet domains{kDomainNone};  // 参照したい範囲
    bool              includeCapability{false};
};

struct PrinterStatus
{
    std::optional<MachineSnapshot> data;        // DataStore現在値(読取専用)
    std::optional<CapabilitySet>   capability;  // 現在Capability(読取専用)
};

} // namespace rim
