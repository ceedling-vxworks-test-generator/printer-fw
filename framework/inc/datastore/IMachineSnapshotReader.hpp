#pragma once

//
// IMachineSnapshotReader - Capability/Accessor が現在状態を取得する唯一のIF(仕様 §6.11)。
// Registry 内部実装を隠蔽する。
//

#include "datastore/Snapshot.hpp"

namespace rim
{

class IMachineSnapshotReader
{
public:

    virtual ~IMachineSnapshotReader() = default;

    virtual MachineSnapshot Capture(const SnapshotRequest& request) const = 0;
};

} // namespace rim
