#pragma once

//
// Accessor - 隣接 Accessor Layer(RIM4レイヤ外・参照専用/Pull)。
// rim_core.cpp の rim_accessor_* を移植。L2/L3 から都度取得して返す。
//

#include "datastore/DataStore.hpp"
#include "datastore/Snapshot.hpp"
#include "datastore/DomainSet.hpp"
#include "datastore/RIMResult.hpp"

#include "capability/CapabilitySet.hpp"
#include "capability/CapabilityManager.hpp"

namespace rim
{

class Accessor
{
public:

    Accessor(DataStore& store, CapabilityManager& capability)
        : store_(store)
        , capability_(capability)
    {
    }

    // 現在の機械Snapshotを都度取得(保持しない)。
    RIMResult ReadSnapshot(DomainSet domains, MachineSnapshot& out) const
    {
        SnapshotRequest req;
        req.domains = domains;
        return store_.Capture(req, out);
    }

    // 現在のCapabilityを都度取得。
    bool ReadCapability(CapabilitySet& out) const
    {
        return capability_.Current(out);
    }

private:

    DataStore&         store_;
    CapabilityManager& capability_;
};

} // namespace rim
