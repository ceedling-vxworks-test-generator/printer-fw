#pragma once

#include "IProductProvider.hpp"
#include "../PrinterAProductProfile.hpp"

#include "../PrinterAProductDefinition.hpp"
#include "DeliveryHandler/PrinterACustomDeliveryHandler.hpp"

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
    ICustomDeliveryHandler*
    GetCustomDeliveryHandler() override
    {
        return &customDeliveryHandler_;
    }

private:

    PrinterACustomDeliveryHandler
        customDeliveryHandler_;

};

} // namespace rim