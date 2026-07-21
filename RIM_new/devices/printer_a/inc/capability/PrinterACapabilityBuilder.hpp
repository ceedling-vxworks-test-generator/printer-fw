#pragma once

//
// PrinterACapabilityBuilder - PrinterA の8種Capability判定(機種可変)。ICapabilityBuilder 実装。
// 各Capは Snapshot(観測事実)のみに依存し、他Capを参照しない(PrintCapも DataStore値のみ)。
//

#include "capability/ICapabilityBuilder.hpp"

namespace rim
{

class PrinterACapabilityBuilder final
    : public ICapabilityBuilder
{
public:

    CapabilitySet Build(const MachineSnapshot& snapshot) const override;
};

} // namespace rim
