#pragma once

//
// PrinterAEvaluatorSet - PrinterA の Capability 判定式(機種可変)。ICapabilityEvaluatorSet 実装。
// rim_capability.cpp の eval_printable / eval_temp_alert を移植。
//   - printable  : activeなfaultが無ければ印刷可
//   - temp_alert : 温度 > 60.00℃(CelsiusX100 > 6000) で警報
//

#include "capability/ICapabilityEvaluatorSet.hpp"

#include "datastore/RIMDataId.hpp"
#include "datastore/ValueType.hpp"

namespace rim
{

class PrinterAEvaluatorSet final
    : public ICapabilityEvaluatorSet
{
public:

    void Evaluate(
        const MachineSnapshot& snapshot,
        CapabilitySet& out) const override
    {
        EvalPrintable(snapshot, out);
        EvalTempAlert(snapshot, out);
    }

private:

    static void EvalPrintable(const MachineSnapshot& s, CapabilitySet& out)
    {
        bool noFault = true;
        if (s.fault && s.fault->activeCount > 0) noFault = false;
        out.printable = noFault;
    }

    static void EvalTempAlert(const MachineSnapshot& s, CapabilitySet& out)
    {
        bool alert = false;
        if (s.current && s.current->present[ToIndex(RIMDataId::kTemperature)]) {
            const RIMValue& v = s.current->values[ToIndex(RIMDataId::kTemperature)];
            if (v.type == ValueType::kCelsiusX100 && v.u.celsiusX100 > 6000)
                alert = true;
        }
        out.tempAlert = alert;
    }
};

} // namespace rim
