#pragma once

//
// MachineSnapshotReader - IMachineSnapshotReader 実装(仕様 §6.11)。
// MachineRegistry 経由で要求ドメインのSnapshotを生成する。排他は各Registry内で行う。
//

#include "datastore/IMachineSnapshotReader.hpp"
#include "datastore/MachineRegistry.hpp"

namespace rim
{

class MachineSnapshotReader final
    : public IMachineSnapshotReader
{
public:

    explicit MachineSnapshotReader(MachineRegistry& registry)
        : registry_(registry)
    {
    }

    MachineSnapshot Capture(const SnapshotRequest& request) const override
    {
        return registry_.Capture(request.domains);
    }

private:

    MachineRegistry& registry_;
};

} // namespace rim
