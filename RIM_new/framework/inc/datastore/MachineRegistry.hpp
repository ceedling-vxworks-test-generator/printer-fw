#pragma once

//
// MachineRegistry - 3 Registry を集約するファサード(仕様 §6.8)。
// Dispatcher は各 Registry を、MachineSnapshotReader は本ファサードの Capture を用いる。
//

#include "datastore/FaultRegistry.hpp"
#include "datastore/OperationRegistry.hpp"
#include "datastore/CurrentValueRegistry.hpp"
#include "datastore/Snapshot.hpp"
#include "datastore/RegistryDomain.hpp"

namespace rim
{

class MachineRegistry
{
public:

    FaultRegistry&        Fault()        { return fault_; }
    OperationRegistry&    Operation()    { return operation_; }
    CurrentValueRegistry& CurrentValue() { return currentValue_; }

    // 要求ドメインのSnapshotを集約して返す。
    MachineSnapshot Capture(RegistryDomainSet domains) const
    {
        MachineSnapshot ms;
        if (domains & kDomainFault)      ms.fault     = fault_.MakeSnapshot();
        if (domains & kDomainOperation)  ms.operation = operation_.MakeSnapshot();
        if (domains & kDomainCurrentAll) ms.currentValue = currentValue_.MakeSnapshot(domains);
        return ms;
    }

    void Clear()
    {
        fault_.Clear();
        operation_.Clear();
        currentValue_.Clear();
    }

private:

    FaultRegistry        fault_;
    OperationRegistry    operation_;
    CurrentValueRegistry currentValue_;
};

} // namespace rim
