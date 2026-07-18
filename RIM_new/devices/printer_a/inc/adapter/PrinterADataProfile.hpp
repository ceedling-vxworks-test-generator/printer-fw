#pragma once

//
// PrinterADataProfile - PrinterA の id 定義(機種可変)。IDataProfile 実装。
// rim_adapter.cpp の classify()/convert() を PrinterA 固有の正規化として移植。
//

#include "adapter/IDataProfile.hpp"

namespace rim
{

class PrinterADataProfile final
    : public IDataProfile
{
public:

    std::optional<InputKind> Classify(RIMDataId id) const override;

    std::optional<RIMValue> Convert(
        RIMDataId id,
        double raw,
        const DataContext& ctx) const override;
};

} // namespace rim
