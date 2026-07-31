#pragma once

#include "IProductProvider.hpp"

#include "../DataItem/PrinterADataDefinitionProvider.hpp"
#include "../CapabilityItem/PrinterAErrorRegistry.hpp"
#include "../PrinterAProductDefinition.hpp"

namespace rim
{

class PrinterAProvider final
    : public IProductProvider
{
public:

    const IErrorDefinitionRegistry&
    GetErrorRegistry() const override
    {
        return errorRegistry_;
    }

    const IDataDefinitionProvider&
    GetDataDefinitionProvider() const override
    {
        return dataDefinitionProvider_;
    }

    const ProductDefinition&
    GetProductDefinition() const override
    {
        return kPrinterAProductDefinition;
    }

private:

    PrinterAErrorRegistry errorRegistry_;
    PrinterADataDefinitionProvider dataDefinitionProvider_;
};

} // namespace rim