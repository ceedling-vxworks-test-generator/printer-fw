#pragma once

//
// PrinterStatusReader - Accessor Layer(仕様 §7.1)。参照専用・Pull。状態を保持しない。
// DataStore(MachineSnapshotReader経由)と現在Capabilityを都度取得して返す。
//

#include "accessor/PrinterStatus.hpp"

#include "datastore/IMachineSnapshotReader.hpp"
#include "capability/CapabilityManager.hpp"

namespace rim
{

class PrinterStatusReader
{
public:

    PrinterStatusReader(IMachineSnapshotReader& reader, const CapabilityManager& capability)
        : reader_(reader), capability_(capability)
    {
    }

    PrinterStatus GetPrinterStatus(const PrinterStatusRequest& request) const
    {
        PrinterStatus status;

        SnapshotRequest req;
        req.domains = request.domains;
        status.data = reader_.Capture(req);

        if (request.includeCapability) {
            CapabilitySet cap;
            if (capability_.Current(cap)) status.capability = cap;
        }
        return status;
    }

private:

    IMachineSnapshotReader&   reader_;
    const CapabilityManager&  capability_;
};

} // namespace rim
