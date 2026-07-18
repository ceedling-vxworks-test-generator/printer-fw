#pragma once

//
// Snapshot 群 - 読み取り専用・生成後不変・単一時点整合性。
// rim_datastore.h の rim_*_snapshot_t / rim_snapshot_request_t 相当。
// rim の has_xxx は std::optional で表現。
//

#include <array>
#include <cstdint>
#include <optional>

#include "datastore/Config.hpp"
#include "datastore/RIMDataId.hpp"
#include "datastore/RIMValue.hpp"
#include "datastore/DomainSet.hpp"

namespace rim
{

struct FaultSnapshot
{
    std::uint32_t activeCount{0};
    std::array<std::uint32_t, kFaultCap> activeCodes{};
};

struct CurrentValueSnapshot
{
    std::array<bool, kRIMDataIdCount>     present{};
    std::array<RIMValue, kRIMDataIdCount> values{};
};

struct MachineSnapshot
{
    std::optional<FaultSnapshot>        fault;
    std::optional<CurrentValueSnapshot> current;
};

struct SnapshotRequest
{
    DomainSet domains{kDomainNone};
};

} // namespace rim
