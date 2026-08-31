#pragma once

#include "IProductProvider.hpp"
#include "../PrinterAProductProfile.hpp"

#include "../PrinterAProductDefinition.hpp"

namespace rim
{

class PrinterAProvider final
    : public IProductProvider
{
public:

    const ProductProfile&
    GetProfile() const override
    {
        return kPrinterAProductProfile;
    }
};

} // namespace rim